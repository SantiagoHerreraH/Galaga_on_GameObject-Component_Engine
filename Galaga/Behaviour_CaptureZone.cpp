#include "Behaviour_CaptureZone.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include "Scene.h"
#include "SwirlData.h"
#include "WaveMovement.h"
#include "GameTime.h"
#include "CAnimation.h"
#include "CCollider.h"
#include "CollisionLayers.h"
#include "CGameStatController.h"
#include "Settings.h"
#include "MathTools.h"
#include "WeaponType.h"
#include "CCaptureZone.h"

dae::MovementActionSequenceHandle dae::CaptureZoneBehaviour::CreateInstance(Enemy& enemyCreator)

{
	GameObjectHandle enemy{ enemyCreator.GetGameObjectHandle() };
	GameObjectHandle player{ enemyCreator.GetEnemyInstanceData().Player };
	GameObjectHandle grid{ enemyCreator.GetEnemyInstanceData().Grid };
	Scene& scene{ *enemyCreator.GetEnemyInstanceData().Scene };
	glm::vec3 relativePos{ };
	relativePos.x = enemyCreator.GetEnemyInstanceData().RelativePos.x;
	relativePos.y = enemyCreator.GetEnemyInstanceData().RelativePos.y;
	bool goTowardsLeft = enemyCreator.GetEnemyInstanceData().MoveTowardsLeft;

	CaptureZoneData captureZoneData{};

	captureZoneData.OnActivateAudioData.File = "Sound/TractorBeam.wav";
	captureZoneData.OnActivateAudioData.LoopAmount = 0;

	captureZoneData.OnHitAudioData.File = "Sound/TractorBeamCaptured.wav";
	captureZoneData.OnHitAudioData.LoopAmount = 0;

	captureZoneData.RelativePos = glm::vec3{ 0,50,0 };
	captureZoneData.Target = player;

	auto captureZone = enemy->AddComponent(CCaptureZone{ captureZoneData });

	//------ PARENTING

	auto unParent = [enemy]() mutable {
		enemy->Transform().MakeRootNode();
		};

	auto reParent = [enemy, grid, relativePos]() mutable {
		enemy->Transform().SetParent(*grid, ETransformReparentType::KeepLocalTransform);
		enemy->Transform().OverrideWorldScaleWithLocalScale(true);
		enemy->Transform().SetLocalPosition(relativePos);
		};


	CMovementActionSequence captureZoneBehavior{ "CaptureZoneBehavior" };

	captureZoneBehavior.AddConditionToStartSequence([captureZone]() {

		return !captureZone->CapturedTarget();
		});

	//--------------------- Move towards enemy
	const float movementDuration = 1;
	const float targetDistanceFromPlayer = 50;

	captureZoneBehavior.AddAction("Move Towards Enemy");
	captureZoneBehavior.AddStartSubAction(unParent);
	captureZoneBehavior.SetActionDependsOnDuration(true, movementDuration);
	captureZoneBehavior.SetMovementPathDecidingFunction([enemy, player, targetDistanceFromPlayer]() mutable {

		glm::vec2 goalPos{ player->Transform().GetWorldTransform().Position };
		goalPos.y -= targetDistanceFromPlayer;

		glm::vec2 movementPath = goalPos - (glm::vec2)enemy->Transform().GetWorldTransform().Position;

		return movementPath;
		});


	captureZoneBehavior.AddActionFunction([movementDuration, enemy](float timeSinceStarted, const MovementData& data) mutable {

		glm::vec2 pos = data.BeginningPosition + (data.BeginningDeltaTowardsTarget * (timeSinceStarted / movementDuration));

		glm::vec2 normalizedDelta{ glm::normalize(data.BeginningDeltaTowardsTarget) };

		enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(-normalizedDelta.x, normalizedDelta.y)));
		enemy->Transform().SetLocalPositionX(pos.x);
		enemy->Transform().SetLocalPositionY(pos.y);

		return false;

		});

	//--------------------- Activate CaptureZone


	const float captureZoneDuration = 3;
	captureZoneBehavior.AddAction("Activate Capture Zone");
	captureZoneBehavior.SetActionDependsOnDuration(true, captureZoneDuration);
	captureZoneBehavior.AddStartSubAction([captureZone, enemy]() mutable {

		captureZone->SetActiveCaptureZone(true);
		enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(0, 1)));

		});
	captureZoneBehavior.AddEndSubAction([captureZone]() mutable {

		captureZone->SetActiveCaptureZone(false);

		});

	//------------------- Loop Back to Formation


	const float speedToReturn = 120;// m/s

	const int xBoundsOffsetFromWindowWidth{ 10 };
	const int yBoundsOffsetFromWindowHeight{ 10 };

	const int xBounds{ xBoundsOffsetFromWindowWidth + g_WindowWidth };
	const int yBounds{ yBoundsOffsetFromWindowHeight + g_WindowHeight };

	captureZoneBehavior.AddAction("Loop Back To Formation");
	captureZoneBehavior.SetActionDependsOnDuration(false);

	captureZoneBehavior.AddConditionToStartOrRestartAction([captureZone]() {

		//bool startCondition = !(*isPlayerCaptured);

		//(*isPlayerCaptured) = false;

		return !captureZone->CapturedTarget(); // start action if player is not captured

		});

	captureZoneBehavior.SetMovementPathDecidingFunction([]() {return glm::vec2{ 0,1 }; }); //just to calculate begin pos

	captureZoneBehavior.AddActionFunction([
		enemy,
		grid,
		relativePos,
		speedToReturn,
		reParent,
		xBoundsOffsetFromWindowWidth,
		yBoundsOffsetFromWindowHeight,
		xBounds,
		yBounds
	]
		(float,
			const MovementData& data) mutable
		{

			enemy->Transform().MoveLocalPositionY(speedToReturn * GameTime::GetInstance().GetElapsedSeconds());

			glm::vec2 endPos{ enemy->Transform().GetWorldTransform().Position };

			//looping
			if (MathTools::IsOutOfBounds((int)endPos.x, -xBoundsOffsetFromWindowWidth, xBounds) ||
				MathTools::IsOutOfBounds((int)endPos.y, -yBoundsOffsetFromWindowHeight, yBounds))
			{
				endPos = data.BeginningPosition;
				endPos.y = 0;

				enemy->Transform().SetLocalPositionX(endPos.x);
				enemy->Transform().SetLocalPositionY(endPos.y);

				return true;
			}

			return false;

		});

		//------------------- Go Back to Formation


		const float radiusToAttach = 7;
		captureZoneBehavior.AddAction("Go Back To Formation");
		captureZoneBehavior.SetActionDependsOnDuration(false);

		captureZoneBehavior.AddActionFunction([enemy, grid, relativePos, speedToReturn, radiusToAttach, reParent]
		(float,
			const MovementData&) mutable
			{
				glm::vec3 delta{ grid->Transform().LocalToWorldVec(relativePos) - enemy->Transform().GetWorldTransform().Position };

				if (glm::length(delta) > radiusToAttach)
				{

					delta = glm::normalize(delta) * speedToReturn * GameTime::GetInstance().GetElapsedSeconds();

					enemy->Transform().MoveLocalPositionX(delta.x);
					enemy->Transform().MoveLocalPositionY(delta.y);

					return false;
				}

				reParent();
				return true;

			});

		return enemy->AddComponent(captureZoneBehavior);
}



