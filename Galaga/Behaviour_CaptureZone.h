#pragma once


#define _USE_MATH_DEFINES
#include <cmath>
#include "Enemy.h"
#include "MovementActionSequence.h"
#include "Scene.h"
#include "SwirlData.h"
#include "WaveMovement.h"
#include "GameTime.h"
#include "Animation.h"
#include "Collider.h"
#include "CollisionLayers.h"
#include "GalagaStats.h"
#include "Settings.h"
#include "MathTools.h"

namespace dae {

	GameObjectHandle CreatePlayerDummy(GameObjectHandle parent, dae::Scene& scene, const glm::vec3& relativePos)
	{
		//-----

		dae::TransformData currentTransformData{};
		currentTransformData.Position = relativePos;

		//-----

		TransformData textureTransform{};
		textureTransform.Scale = { 0.5f, 0.5f , 0.5f };
		dae::CTextureHandle currentTexture{ "galaga.png" };
		currentTexture.SetTextureTransform(textureTransform);
		currentTexture.Center();

		dae::GameObjectHandle dummy = scene.CreateGameObject();
		dummy->Transform().SetLocalPosition(relativePos);
		dummy->Transform().OverrideWorldScaleWithLocalScale(true);
		dummy->AddComponent(currentTexture);

		MovementActionSequence actionSequence{ "Player Dummy Movement Sequence" };

		float acceptableRadius = 10;
		float targetDistanceFromParentY = -35;
		float movementDuration = 1.f;
		float rotationSpeed = 360.0f;

		actionSequence.AddAction("Move");
		actionSequence.SetActionDependsOnDuration(false);
		actionSequence.AddActionFunction([dummy, movementDuration, parent, targetDistanceFromParentY, acceptableRadius](float, const MovementData&) mutable {

			glm::vec2 goalPos{ parent->Transform().GetWorldTransform().Position };
			goalPos.y += targetDistanceFromParentY;

			glm::vec2 movementPath = goalPos - (glm::vec2)dummy->Transform().GetWorldTransform().Position;

			if (glm::length(movementPath) <= acceptableRadius)
			{
				return true;
			}

			glm::vec2 pos = movementPath * (Time::GetInstance().GetElapsedSeconds() / movementDuration);

			dummy->Transform().MoveLocalPositionX(pos.x);
			dummy->Transform().MoveLocalPositionY(pos.y);

			return false;

			});
		actionSequence.AddActionFunction([dummy, rotationSpeed](float, const MovementData&) mutable {

			float rotationDelta = rotationSpeed * Time::GetInstance().GetElapsedSeconds();

			dummy->Transform().MoveLocalRotationZ(rotationDelta);

			return false;

			});
		actionSequence.AddEndSubAction([parent, dummy, targetDistanceFromParentY]() mutable {

			dummy->Transform().SetParent(*parent, ETransformReparentType::KeepWorldTransform);
			dummy->Transform().SetLocalPositionY(targetDistanceFromParentY);
			dummy->Transform().SetLocalRotationZ(180.0f);
			});

		dummy->AddComponent(actionSequence);

		return dummy;
	}

	//Make this a componente -> Damage zone component and make a function to activate and deactivate
	GameObjectHandle CreateCaptureZone(GameObjectHandle& parent, GameObjectHandle player, dae::Scene& scene, const glm::vec3& relativePos, GameObjectHandle& outPlayerDummny, std::shared_ptr<bool>& outCapturedEnemy)
	{
		TimerSystem* TIMERSYSTEM{ &TimerSystem::GetFromScene(&scene) };

		//------

		SpriteSheet spriteSheet{ dae::CTextureHandle{"beam.png"}, 1, 3 };
		spriteSheet.TextureHandle().Center();

		CAnimation animation{};
		animation.SetFramesPerSecond(6);
		animation.AddFrame(spriteSheet.GetFrame(0, 0), 1);
		animation.AddFrame(spriteSheet.GetFrame(0, 1), 1);
		animation.AddFrame(spriteSheet.GetFrame(0, 2), 1);

		//------

		Rect rect{};
		rect.Height = (int)spriteSheet.GetScaledCellHeight();
		rect.Width = (int)spriteSheet.GetScaledCellWidth();

		CCollider collider{rect, (int)GalagaCollisionLayers::Bullets};
		collider.CenterRect();

		//------ CREATE GAMEOBJECT

		GameObjectHandle captureZone = scene.CreateGameObject();
		captureZone->Transform().SetLocalPosition(relativePos);
		captureZone->AddComponent(animation);
		captureZone->AddComponent(collider);

		captureZone->Transform().SetParent(*parent, ETransformReparentType::KeepLocalTransform);
		captureZone->SetActive(false);


		GameObjectHandle playerDummy = CreatePlayerDummy(parent, scene, glm::vec3{});
		playerDummy->SetActive(false);

		int captureZoneDamage = -1;

		float secondsToWaitBeforeOffsettingStat = 1.f;

		Timer timer{ secondsToWaitBeforeOffsettingStat, true };
		timer.GetOnEndEvent().Subscribe([player, captureZoneDamage, TIMERSYSTEM]() mutable {

			CStatController* statController = player->GetComponent<CStatController>();

			if (statController)
			{
				statController->OffsetStat(StatType::Health, captureZoneDamage);
			}

			});
		TimerKey offsetStatAfterTime_TimerKey = TIMERSYSTEM->AddTimer(timer);

		auto onCaptureZoneCollision = [TIMERSYSTEM, player, playerDummy, offsetStatAfterTime_TimerKey]() mutable {

			player->SetActive(false);//depending on how you code it, this might not work since it will be activated later thanks to the offset stat revival

			playerDummy->Transform().MakeRootNode();
			playerDummy->Transform().SetLocalPosition(player->Transform().GetWorldTransform().Position);
			playerDummy->SetActive(true);

			playerDummy->GetComponent<MovementActionSequence>()->StartSequence();
			TIMERSYSTEM->RestartTimer(offsetStatAfterTime_TimerKey);

			};

		//So you don't deactivate stuff on collision stay
		TimerKey onCaptureCollisionTimerKey = TIMERSYSTEM->TriggerFunctionAfterSeconds(onCaptureZoneCollision, 0.05f, false, true);

		captureZone->GetComponent<CCollider>()->OnCollisionBeginEvent().Subscribe([TIMERSYSTEM, player, onCaptureCollisionTimerKey, outCapturedEnemy](
			const GameObject&,
			const GameObject& other) mutable {

				if (&other == player.get())
				{
					(*outCapturedEnemy) = true;
					TIMERSYSTEM->RestartTimer(onCaptureCollisionTimerKey);
				}

			});

		outPlayerDummny = playerDummy;

		return captureZone;
	}

	class CaptureZoneBehaviour final : public EnemyBehaviour {

	public:

		MovementActionSequenceHandle CreateInstance(Enemy& enemyCreator) override
		{
			GameObjectHandle enemy{ enemyCreator.GetGameObjectHandle() };
			GameObjectHandle player{ enemyCreator.GetEnemyInstanceData().Player };
			GameObjectHandle grid{ enemyCreator.GetEnemyInstanceData().Grid };
			Scene& scene{ *enemyCreator.GetEnemyInstanceData().Scene };
			glm::vec3 relativePos{ };
			relativePos.x = enemyCreator.GetEnemyInstanceData().RelativePos.x;
			relativePos.y = enemyCreator.GetEnemyInstanceData().RelativePos.y;
			bool goTowardsLeft = enemyCreator.GetEnemyInstanceData().MoveTowardsLeft;

			std::shared_ptr<bool> isPlayerCaptured{ std::make_shared<bool>(false) };
			GameObjectHandle playerDummy = scene.CreateGameObject();
			GameObjectHandle captureZone = CreateCaptureZone(enemy, player, scene, glm::vec3{ 0,50,0 }, playerDummy, isPlayerCaptured);

			enemyCreator.OnDie().Subscribe([playerDummy, captureZone]() mutable {
				playerDummy->SetActive(false);
				captureZone->SetActive(false);
				});

			//------ PARENTING

			auto unParent = [enemy]() mutable {
				enemy->Transform().MakeRootNode();
				};

			auto reParent = [enemy, grid, relativePos]() mutable {
				enemy->Transform().SetParent(*grid, ETransformReparentType::KeepLocalTransform);
				enemy->Transform().OverrideWorldScaleWithLocalScale(true);
				enemy->Transform().SetLocalPosition(relativePos);
				};


			MovementActionSequence captureZoneBehavior{  "CaptureZoneBehavior" };

			captureZoneBehavior.AddConditionToStartSequence([playerDummy]() {

				return !playerDummy->IsActive();
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

				captureZone->SetActive(true);
				enemy->Transform().SetLocalRotationZ(glm::degrees(std::atan2f(0, 1)));

				});
			captureZoneBehavior.AddEndSubAction([captureZone]() mutable {

				captureZone->SetActive(false);

				});

			//------------------- Loop Back to Formation


			const float speedToReturn = 120;// m/s

			const int xBoundsOffsetFromWindowWidth{ 10 };
			const int yBoundsOffsetFromWindowHeight{ 10 };

			const int xBounds{ xBoundsOffsetFromWindowWidth + g_WindowWidth };
			const int yBounds{ yBoundsOffsetFromWindowHeight + g_WindowHeight };

			captureZoneBehavior.AddAction("Loop Back To Formation");
			captureZoneBehavior.SetActionDependsOnDuration(false);

			captureZoneBehavior.AddConditionToStartOrRestartAction([isPlayerCaptured]() {

				bool startCondition = !(*isPlayerCaptured);

				(*isPlayerCaptured) = false;

				return startCondition; // start action if player is not captured

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

					enemy->Transform().MoveLocalPositionY(speedToReturn * Time::GetInstance().GetElapsedSeconds());

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

							delta = glm::normalize(delta) * speedToReturn * Time::GetInstance().GetElapsedSeconds();

							enemy->Transform().MoveLocalPositionX(delta.x);
							enemy->Transform().MoveLocalPositionY(delta.y);

							return false;
						}

						reParent();
						return true;

					});

				return enemy->AddComponent(captureZoneBehavior);
		}

	};


}