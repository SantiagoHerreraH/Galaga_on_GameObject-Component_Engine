#pragma once

#include <limits>

#include "Scene.h"
#include "PlayerController.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Movement.h"
#include "Texture.h"
#include "StatSystem.h"
#include "Text.h"
#include "TimerSystem.h"

#include "GalagaStats.h"
#include "CollisionLayers.h"
#include "PlayerScore.h"
#include "Bullets.h"


namespace dae {

	class GalagaPlayer
	{
	};

	dae::GameObjectHandle CreatePlayer(
		dae::Scene& scene,
		const glm::vec2& startPos,
		float zRotation,
		std::shared_ptr<Event<>> onGameEnd,
		std::shared_ptr<Event<>> stopEnemies = nullptr,
		std::shared_ptr<Event<>> startEnemies = nullptr)
	{


		//------- SHOOTING PIVOT
		dae::GameObjectHandle shootingPivot = scene.CreateGameObject();
		shootingPivot->Transform().SetLocalPositionY(-10);

		//------- PLAYER

		dae::Scene* scenePtr = &scene;
		TimerSystem* TIMERSYSTEM = &TimerSystem::GetFromScene(scenePtr);
		dae::GameObjectHandle currentPlayer = scene.CreateGameObject();


		//----- COMPONENTS -----

		Rect rect{};
		rect.Height = 32;
		rect.Width = 32;

		dae::CCollider collider{ rect, (int)GalagaCollisionLayers::Player};
		collider.CenterRect();

		currentPlayer->AddComponent(collider);
		
		//-----

		dae::CRigidbody2D rigidBody{};
		currentPlayer->AddComponent(rigidBody);

		//-----

		dae::CMovement2D movement{};
		movement.SetMaxSpeed(300);
		currentPlayer->AddComponent(movement);

		//-----

		TransformData textureTransform{};
		textureTransform.Scale = { 0.5f, 0.5f , 0.5f };
		dae::CTextureHandle currentTexture{ "galaga.png" };
		currentTexture.SetTextureTransform(textureTransform);
		currentTexture.Center();

		currentPlayer->AddComponent(currentTexture);

		//-----

		CStatController statController{};

		IntStat initialHealth{ 3, 3, 3 };


		const int maxInt = std::numeric_limits<int>::max();

		IntStat initialPoints{ 0, maxInt, maxInt };
		IntStat initialShotsFired{ 0, maxInt, maxInt };
		IntStat initialNumberOfHits{ 0, maxInt, maxInt };
		dae::GameObjectHandle currentPlayerScore{ CreateCurrentScore(scene) };

		initialPoints.OnCurrentStatChange.Subscribe([currentPlayerScore](int statValue) mutable {
			currentPlayerScore->GetComponent<CText>()->SetText(std::to_string(statValue));
			});

		statController.CreateStat(StatType::Health, initialHealth);

		statController.CreateStat(StatType::Points, initialPoints);
		statController.CreateStat(StatType::ShotsFired, initialShotsFired);
		statController.CreateStat(StatType::NumberOfHits, initialNumberOfHits);

		currentPlayer->AddComponent(statController);

		//-----

		PlayerController playerController{};
		PlayerId playerId = playerController.GetPlayerId();

		//-----

		currentPlayer->SetActive(false);

		shootingPivot->Transform().SetParent(*currentPlayer, dae::ETransformReparentType::KeepLocalTransform);

		currentPlayer->Transform().SetLocalPositionX(startPos.x);
		currentPlayer->Transform().SetLocalPositionY(startPos.y);
		currentPlayer->Transform().SetLocalRotationZ(zRotation);


		//---- SET STAT EVENTS ----

		CStatController* currentStatController = currentPlayer->GetComponent<CStatController>();
		Event<>& onHealthZeroOrLess = currentStatController->OnCurrentStatZeroOrLess(StatType::Health);
		Event<int>& onHealthChange = currentStatController->OnCurrentStatChange(StatType::Health);

		std::vector<dae::GameObjectHandle> healthIndicators = 
			CreatePlayerHealthIndicator(
				scene,
				playerId,
				initialHealth.MaxStat,
				glm::vec2{g_WindowWidth * 4.3f / 5.f,
				(g_WindowHeight * 2.3f / 5.f) + (playerId * 100)}, 1, 1);

		auto changeHealthIndicators = [healthIndicators](int currentHealth) mutable
			{
				if (currentHealth < 0)
				{
					currentHealth = 0;
				}
				for (int i = 0; i < currentHealth; i++)
				{
					healthIndicators[i]->SetActive(true);
				}
				for (int i = currentHealth; i < healthIndicators.size(); i++) //cast to size_t in case it is negative
				{
					healthIndicators[i]->SetActive(false);
				}
			};

		onHealthChange.Subscribe(changeHealthIndicators);

		glm::vec2 respawnPoint{ startPos };
		auto respawnPlayer = [currentPlayer, respawnPoint, healthIndicators, startEnemies]() mutable
			{
				currentPlayer->Transform().SetLocalPositionX(respawnPoint.x);
				currentPlayer->Transform().SetLocalPositionY(respawnPoint.y);
				currentPlayer->SetActive(true);
				if (startEnemies)
				{
					startEnemies->Invoke();
				}
			};

		float secondsToRespawn = 2.5f;
		TimerKey respawnKey = TIMERSYSTEM->TriggerFunctionAfterSeconds(respawnPlayer, secondsToRespawn, false, true);

		auto despawnAndRespawnAfterTime = [currentPlayer, respawnKey, stopEnemies, TIMERSYSTEM](int) mutable {
			currentPlayer->SetActive(false);
			if (stopEnemies)
			{
				stopEnemies->Invoke();
			}
			TIMERSYSTEM->RestartTimer(respawnKey);
			};


		onHealthChange.Subscribe(despawnAndRespawnAfterTime); //YOU SHOULD ALSO SUBSCRIBE THE STOP MOVEMENT ENEMY HERE
		onHealthZeroOrLess.Subscribe([onGameEnd]()mutable {onGameEnd->Invoke(); });
		onHealthZeroOrLess.Subscribe([currentPlayer]() mutable {

			CStatController* currentStatController = currentPlayer->GetComponent<CStatController>();
			currentStatController->ResetCurrentStat(StatType::Health);
			});

		//----- ACTIONS -----

		// - Movement Action
		auto moveLeft = [currentPlayer]() mutable {currentPlayer->GetComponent<CMovement2D>()-> AddSingleFrameMovementInput(glm::vec2{ -1,  0 }); };
		auto moveRight = [currentPlayer]() mutable {currentPlayer->GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 1,  0 }); };
		auto moveUp = [currentPlayer]() mutable {currentPlayer->GetComponent<CMovement2D>()->   AddSingleFrameMovementInput(glm::vec2{ 0, -1 }); };
		auto moveDown = [currentPlayer]() mutable {currentPlayer->GetComponent<CMovement2D>()-> AddSingleFrameMovementInput(glm::vec2{ 0,  1 }); };


		// - Shooting Action
		const int bulletNumber = 10;
		int currentBulletIndex = 0;

		std::vector<GameObjectHandle> bullets{};
		std::vector<size_t> bulletLifeTimerKeys{};

		Timer timeBetweenShots{ .3f, false };
		TimerKey timerKey = TIMERSYSTEM->AddTimer(timeBetweenShots);

		bulletLifeTimerKeys.resize(bulletNumber);

		for (size_t i = 0; i < bulletNumber; i++)
		{
			bullets.push_back(CreateBullets(scene, currentPlayer, bulletLifeTimerKeys[i]));
		}

		auto shootAction = [currentPlayer, shootingPivot, bullets, bulletLifeTimerKeys, currentBulletIndex, timerKey, TIMERSYSTEM]() mutable
			{

				if (currentPlayer->IsActive() && TIMERSYSTEM->TimerAt(timerKey).IsFinished())
				{
					currentPlayer->GetComponent<CStatController>()->OffsetStat(StatType::ShotsFired, 1);
					bullets[currentBulletIndex]->Transform().SetLocalPosition(shootingPivot->Transform().GetWorldTransform().Position);

					bullets[currentBulletIndex]->SetActive(true);

					TIMERSYSTEM->RestartTimer(timerKey);//time between shots
					TIMERSYSTEM->RestartTimer(bulletLifeTimerKeys[currentBulletIndex]);//bullet lifetime

					currentBulletIndex = (currentBulletIndex + 1) % bullets.size();
				}

			};

		//Events

		Event<> shootEvent{};
		Event<> moveLeftEvent{};
		Event<> moveRightEvent{};
		Event<> moveUpEvent{};
		Event<> moveDownEvent{};

		moveLeftEvent.Subscribe(moveLeft);
		moveRightEvent.Subscribe(moveRight);
		moveUpEvent.Subscribe(moveUp);
		moveDownEvent.Subscribe(moveDown);
		shootEvent.Subscribe(shootAction);

		playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_SPACE,	 shootEvent });
		playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_LEFT,	 moveLeftEvent });
		playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_RIGHT,	 moveRightEvent });
		playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_UP,		 moveUpEvent });
		playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_DOWN,	 moveDownEvent });

		playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_A,	 moveLeftEvent });
		playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_D,	 moveRightEvent });
		playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_W,	 moveUpEvent });
		playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, SDL_SCANCODE_S,	 moveDownEvent });


		playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::ButtonX,	 shootEvent });
		playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadLeft,	 moveLeftEvent });
		playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadRight,	 moveRightEvent });
		playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadUp,		 moveUpEvent });
		playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_PRESSED, GamepadButton::DpadDown,	 moveDownEvent });


		currentPlayer->AddComponent(playerController);

		return currentPlayer;


}
}