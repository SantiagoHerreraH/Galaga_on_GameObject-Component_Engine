#pragma once


#define _USE_MATH_DEFINES
#include <cmath>
#include "Enemy.h"
#include "MovementActionSequence.h"
#include "Scene.h"
#include "SwirlData.h"
#include "WaveMovement.h"
#include "GameTime.h"
#include "Settings.h"
#include "MathTools.h"

namespace dae {


	class ZigZagDiveBehaviour final : public EnemyBehaviour {

	public:

		virtual MovementActionSequenceHandle CreateInstance(Enemy& enemyCreator) override
		{

			GameObjectHandle enemy{ enemyCreator.GetGameObjectHandle() };
			GameObjectHandle player{ enemyCreator.GetEnemyInstanceData().Player };
			GameObjectHandle grid{ enemyCreator.GetEnemyInstanceData().Grid };
			Scene& scene{ *enemyCreator.GetEnemyInstanceData().Scene };
			glm::vec3 relativePos{ };
			relativePos.x = enemyCreator.GetEnemyInstanceData().RelativePos.x;
			relativePos.y = enemyCreator.GetEnemyInstanceData().RelativePos.y;
			bool goTowardsLeft = enemyCreator.GetEnemyInstanceData().MoveTowardsLeft;

			//------ PARENTING

			auto unParent = [enemy]() mutable {
				enemy->Transform().MakeRootNode();
				};

			auto reParent = [enemy, grid, relativePos]() mutable {
				enemy->Transform().SetParent(*grid, ETransformReparentType::KeepLocalTransform);
				enemy->Transform().OverrideWorldScaleWithLocalScale(true);
				enemy->Transform().SetLocalPosition(relativePos);
				};

			MovementActionSequence zigZagDiveBehavior{ "ZigZagDive" };

			const int xBoundsOffsetFromWindowWidth{ 10 };
			const int yBoundsOffsetFromWindowHeight{ 10 };


			const int xBounds{ xBoundsOffsetFromWindowWidth + g_WindowWidth };
			const int yBounds{ yBoundsOffsetFromWindowHeight + g_WindowHeight };

			//--- 1. Go Towards the player ---

			float orthogonalMovementSpeed{ 10 };
			float orthogonalMovementRange{ 10 };

			float perpendicularMovementSpeed{ 250.f };
			glm::vec2 previousOffset{};

			if (!goTowardsLeft)
			{
				orthogonalMovementSpeed = -orthogonalMovementSpeed;
			}

			zigZagDiveBehavior.AddAction("Dive");
			zigZagDiveBehavior.SetActionDependsOnDuration(false);
			zigZagDiveBehavior.AddStartSubAction(unParent);
			zigZagDiveBehavior.SetMovementPathDecidingFunction([player, enemy]() {

				glm::vec3 direction =
					player->Transform().GetWorldTransform().Position -
					enemy->Transform().GetWorldTransform().Position;

				return direction;
				});

			zigZagDiveBehavior.AddActionFunction([
				enemy,
				orthogonalMovementSpeed,
				orthogonalMovementRange,

				perpendicularMovementSpeed,

				xBounds,
				yBounds,
				xBoundsOffsetFromWindowWidth,
				yBoundsOffsetFromWindowHeight,
				previousOffset](float timeSinceActionStarted, const MovementData& data)mutable

			{

				glm::vec2 offsetFromStartPos{
					WaveMovement::MoveOrthogonallyTowardsTarget(
						orthogonalMovementSpeed,
						orthogonalMovementRange,
						perpendicularMovementSpeed,
						timeSinceActionStarted,
						data.BeginningDeltaTowardsTarget) };

				glm::vec2 delta = offsetFromStartPos - previousOffset;
				previousOffset = offsetFromStartPos;
				glm::vec2 endPos = data.BeginningPosition + offsetFromStartPos;

				//looping
				if (MathTools::IsOutOfBounds((int)endPos.x, -xBoundsOffsetFromWindowWidth, xBounds) ||
					MathTools::IsOutOfBounds((int)endPos.y, -yBoundsOffsetFromWindowHeight, yBounds))
				{
					endPos = data.BeginningPosition;
					endPos.y = 0;

					enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(0, 1)));
					enemy->Transform().SetLocalPositionX(endPos.x);
					enemy->Transform().SetLocalPositionY(endPos.y);

					return true;
				}

				enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(-delta.x, delta.y)));
				enemy->Transform().SetLocalPositionX(endPos.x);
				enemy->Transform().SetLocalPositionY(endPos.y);

				return false;

			});

			//------- Return to Pos -------

			float speedToReturn{ 75 };
			float radiusToAttach{ 10 };


			zigZagDiveBehavior.AddAction("Return To Pos");
			zigZagDiveBehavior.SetActionDependsOnDuration(false);
			zigZagDiveBehavior.AddActionFunction([enemy, grid, relativePos, speedToReturn, radiusToAttach](float, const MovementData&) mutable
				{
					glm::vec3 delta{ grid->Transform().LocalToWorldVec(relativePos) - enemy->Transform().GetWorldTransform().Position };

					if (glm::length(delta) > radiusToAttach)
					{

						delta = glm::normalize(delta) * speedToReturn * Time::GetInstance().GetElapsedSeconds();

						enemy->Transform().MoveLocalPositionX(delta.x);
						enemy->Transform().MoveLocalPositionY(delta.y);

						return false;
					}

					return true;

				});

			zigZagDiveBehavior.AddEndSubAction(reParent);


			return enemy->AddComponent(zigZagDiveBehavior);
		}

	};
}