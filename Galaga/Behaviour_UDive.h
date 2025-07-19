#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include "MovementActionSequence.h"
#include "Scene.h"
#include "Swirls.h"
#include "WaveMovement.h"
#include "GameTime.h"

namespace dae {

	MovementActionSequence AddUDiveBehavior(Scene& scene, GameObjectHandle enemy, GameObjectHandle grid, GameObjectHandle player, const glm::vec3& relativePos, std::shared_ptr<Event<GameObject&>> onEndAction, bool goTowardsLeft) {

		//------ PARENTING

		auto unParent = [enemy]() mutable {
			enemy->Transform().MakeRootNode();
			};

		auto reParent = [enemy, grid, relativePos]() mutable {
			enemy->Transform().SetParent(*grid, ETransformReparentType::KeepLocalTransform);
			enemy->Transform().OverrideWorldScaleWithLocalScale(true);
			enemy->Transform().SetLocalPosition(relativePos);
			};


		MovementActionSequence uDiveBehavior{ scene, enemy, "UDiveBehavior" };

		//-------GO BEHIND THE PLAYER--------

		float orthogonalMovementSpeed{ 2 };
		float orthogonalMovementRange{ 100 };

		float perpendicularMovementSpeed{ 1.f };
		float perpendicularOffsetMovementRange{ 30 };

		float numberOfWaves = 0.5; //think of sine waves  1 wave == circle
		float timeItTakesToCompleteAWave = ((float)M_PI * 2.f) / perpendicularMovementSpeed;
		float totalMovementTime = timeItTakesToCompleteAWave * numberOfWaves;

		glm::vec2 previousOffset{};

		if (!goTowardsLeft)
		{
			orthogonalMovementSpeed = -orthogonalMovementSpeed;
		}


		uDiveBehavior.AddAction("GoBehindThePlayer");
		uDiveBehavior.SetActionDependsOnDuration(true, totalMovementTime);
		uDiveBehavior.AddStartSubAction(unParent);
		uDiveBehavior.SetMovementPathDecidingFunction([enemy, player]() mutable {

			glm::vec3 direction =
				player->Transform().GetWorldTransform().Position -
				enemy->Transform().GetWorldTransform().Position;

			return (glm::vec2)direction;

			});

		uDiveBehavior.AddActionFunction([enemy,
			orthogonalMovementSpeed,
			orthogonalMovementRange,

			perpendicularMovementSpeed,
			perpendicularOffsetMovementRange,
			numberOfWaves,
			previousOffset]
			(float timeSinceStarted, const MovementData& data)mutable

			{

				glm::vec2 offsetFromStartPos{
					WaveMovement::SineMoveTowardsTarget(
						orthogonalMovementSpeed,
						orthogonalMovementRange,
						perpendicularMovementSpeed,
						perpendicularOffsetMovementRange,
						timeSinceStarted,
						data.BeginningDeltaTowardsTarget) };

				glm::vec2 delta{ glm::normalize(offsetFromStartPos - previousOffset) };
				previousOffset = offsetFromStartPos;


				enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(-delta.x, delta.y)));
				enemy->Transform().SetLocalPositionX(data.BeginningPosition.x + offsetFromStartPos.x);
				enemy->Transform().SetLocalPositionY(data.BeginningPosition.y + offsetFromStartPos.y);

				return false;
			});

			//------- RETURN -------

			float speedToReturn{ 50 };
			float radiusToAttach{ 10 };


			uDiveBehavior.AddAction("Return To Pos");
			uDiveBehavior.SetActionDependsOnDuration(false);
			uDiveBehavior.AddActionFunction([enemy, grid, relativePos, speedToReturn, radiusToAttach](float, const MovementData&) mutable
				{
					glm::vec3 delta{ grid->Transform().LocalToWorldVec(relativePos) - enemy->Transform().GetWorldTransform().Position };
					enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(0, 1)));

					if (glm::length(delta) > radiusToAttach)
					{


						delta = glm::normalize(delta) * speedToReturn * Time::GetInstance().GetElapsedSeconds();

						enemy->Transform().MoveLocalPositionX(delta.x);
						enemy->Transform().MoveLocalPositionY(delta.y);

						return false;
					}

					return true;

				});

			uDiveBehavior.AddEndSubAction(reParent);
			uDiveBehavior.AddEndSubAction([onEndAction, enemy]() mutable {
				onEndAction->Invoke(*enemy);
				});

			return uDiveBehavior;
	}

}