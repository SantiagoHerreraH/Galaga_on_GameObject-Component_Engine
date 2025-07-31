#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include "Enemy.h"
#include "MovementActionSequence.h"
#include "Scene.h"
#include "SwirlData.h"
#include "WaveMovement.h"
#include "GameTime.h"

#include <iostream>


namespace dae {

	class SwirlBehaviour final : public EnemyBehaviour
	{
	public:
		SwirlBehaviour(SwirlData swirlData) :
			m_SwirlData(swirlData) {}

		MovementActionSequenceHandle CreateInstance(Enemy& enemyCreator) override
		{
			SwirlData swirlData = m_SwirlData;
			glm::vec3 endRelativePos{};
			endRelativePos.x = enemyCreator.GetEnemyInstanceData().RelativePos.x;
			endRelativePos.y = enemyCreator.GetEnemyInstanceData().RelativePos.y;

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

			auto reParent = [enemy, grid, endRelativePos]() mutable {
				enemy->Transform().SetParent(*grid, ETransformReparentType::KeepLocalTransform);
				enemy->Transform().OverrideWorldScaleWithLocalScale(true);
				enemy->Transform().SetLocalPosition(endRelativePos);
				};


			CMovementActionSequence swirlBehaviour{ "SwirlBehaviour" };

			//-------SPAWN PLAYER--------

			swirlBehaviour.AddAction("Spawn Player");
			swirlBehaviour.SetActionDependsOnDuration(false);
			swirlBehaviour.AddStartSubAction(unParent);
			swirlBehaviour.AddActionFunction([enemy, swirlData]
			(float, const MovementData&)mutable

				{

					enemy->Transform().SetLocalPositionX((float)swirlData.StartWorldPos.x);
					enemy->Transform().SetLocalPositionY((float)swirlData.StartWorldPos.y);
					enemy->SetActive(true, false);

					return true;
				});

			//-------Move Forward--------

			swirlBehaviour.AddAction("MoveTowardsDirection");
			swirlBehaviour.SetActionDependsOnDuration(false);
			swirlBehaviour.SetMovementPathDecidingFunction([swirlData]() mutable {

				return glm::normalize(swirlData.PreSwirlMovementDirection);

				});
			swirlBehaviour.AddActionFunction([enemy, swirlData]
			(float timeSinceActionStarted, const MovementData& data)mutable

				{
					glm::vec normDelta{ glm::normalize(data.BeginningDeltaTowardsTarget) };
					glm::vec currentDelta = normDelta * swirlData.PreSwirlSpeed * timeSinceActionStarted;

					if (glm::length(currentDelta) >= swirlData.PreSwirlGoalDistance)
					{
						return true;
					}

					enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(-normDelta.x, normDelta.y)));
					glm::vec2 currentPos = data.BeginningPosition + currentDelta;
					enemy->Transform().SetLocalPositionX(currentPos.x);
					enemy->Transform().SetLocalPositionY(currentPos.y);

					return false;
				});

			//-------SWIRL--------

			const float averageMovementRange{ (swirlData.SwirlOrthogonalMovementRadius + swirlData.SwirlPerpendicularMovementRadius) / 2.f };
			const float circumference = (2.f * (float)M_PI * averageMovementRange);

			const float timeItTakesToCompleteACircle = circumference / swirlData.SwirlSpeed;// s = m/(m/s)
			const float totalMovementTime = timeItTakesToCompleteACircle * swirlData.SwirlNumberOfCircles;

			glm::vec2 prevOffset{};

			swirlBehaviour.AddAction("Swirl");
			swirlBehaviour.SetActionDependsOnDuration(true, totalMovementTime);
			swirlBehaviour.SetMovementPathDecidingFunction([enemy, grid, endRelativePos]() mutable {

				glm::vec3 direction =
					grid->Transform().LocalToWorldVec(endRelativePos) -
					enemy->Transform().GetWorldTransform().Position;

				return (glm::vec2)direction;

				});

			swirlBehaviour.AddActionFunction([enemy,
				swirlData, prevOffset]
				(float timeSinceStarted, const MovementData& data)mutable

				{

					glm::vec2 offsetBeginnerPos{ data.BeginningPosition };

					glm::vec2 offsetBeginnerPosOffset = glm::normalize(swirlData.SwirlStartPointRelativeToCenter) * swirlData.SwirlPerpendicularMovementRadius;

					offsetBeginnerPos -= offsetBeginnerPosOffset;

					glm::vec2 offsetFromStartPos{
						WaveMovement::ElipticalMovementTowardsTarget(
							swirlData.SwirlSpeed,
							swirlData.SwirlOrthogonalMovementRadius,
							swirlData.SwirlSpeed,
							swirlData.SwirlPerpendicularMovementRadius,
							swirlData.SwirlTimeMultiplier * timeSinceStarted,
							swirlData.SwirlStartPointRelativeToCenter) };

					glm::vec2 normDelta{ glm::normalize(offsetFromStartPos - prevOffset) };
					prevOffset = offsetFromStartPos;

					enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(-normDelta.x, normDelta.y)));
					enemy->Transform().SetLocalPositionX(offsetBeginnerPos.x + offsetFromStartPos.x);
					enemy->Transform().SetLocalPositionY(offsetBeginnerPos.y + offsetFromStartPos.y);

					return false;
				});


			//------- RETURN -------

			swirlBehaviour.AddAction("Return To Pos");
			swirlBehaviour.SetActionDependsOnDuration(false);
			swirlBehaviour.AddActionFunction([enemy, grid, endRelativePos, swirlData](float, const MovementData&) mutable
				{
					glm::vec2 delta{ grid->Transform().LocalToWorldVec(endRelativePos) - enemy->Transform().GetWorldTransform().Position };

					if (glm::length(delta) > swirlData.PostSwirlRadiusToAttach)
					{
						glm::vec2 normDelta{ glm::normalize(delta) };
						enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(-normDelta.x, normDelta.y)));

						delta = normDelta * swirlData.PostSwirlSpeedToReturn * Time::GetInstance().GetElapsedSeconds();

						enemy->Transform().MoveLocalPositionX(delta.x);
						enemy->Transform().MoveLocalPositionY(delta.y);

						return false;
					}


					enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(0, 1)));

					return true;

				});

			swirlBehaviour.AddEndSubAction(reParent);

			return enemy->AddComponent(swirlBehaviour);
		}

	private:
		SwirlData m_SwirlData;

	};

}