#include "PlayerVsScene.h"
#include "Player.h"
#include "Gun.h"
#include "CaptureZoneWeaponType.h"
#include "Settings.h"
#include "PlayerController.h"
#include "Misc_CreationFunctions.h"
#include "HighscoreScene.h"
#include "NameAssignerScene.h"
#include "CCaptureZone.h"
#include "TextCreator.h"
#include "GalagaStats.h"
#include "Collider.h"
#include "PlayerController.h"
#include "MainMenu.h"
#include "EventTriggerCommand.h"

dae::PlayerVsScene::PlayerVsScene()
{
	int xPos = g_WindowWidth / 2.f;
	int yOffset = 50;
	const std::string highscoreSceneName = "PlayerVsHighscoreSceneName";

	//---Player One
	PlayerType playerType{};
	playerType.HasVerticalMovement = false;
	playerType.PlayerCollisionLayer = CollisionLayers::Player;
	playerType.TextureName = "galaga.png";
	playerType.WeaponType = std::make_shared<GunWeaponType>(CollisionLayers::PlayerBullets, CollisionLayers::Enemies);

	glm::vec2 initPosOne{ xPos, g_WindowHeight - yOffset };
	Player playerOne{ initPosOne, 0, 0, playerType };

	//---Player Two

	playerType.HasVerticalMovement = false;
	playerType.PlayerCollisionLayer = CollisionLayers::Enemies;
	playerType.TextureName = "boss.png";
	float bossPlayerSpeed = 350;
	playerType.WeaponType = std::make_shared<CaptureZoneWeaponType>(playerOne.GetGameObjectHandle(), bossPlayerSpeed);

	glm::vec2 initPosTwo{ xPos, yOffset };
	Player playerTwo{ initPosTwo, 0, 1, playerType };

	//---Functions

	auto disableInputAndReset = [playerOne, playerTwo, initPosOne, initPosTwo]() mutable
		{
			playerOne.GetGameObject().GetComponent<CPlayerController>()->SetActive(false);
			playerTwo.GetGameObject().GetComponent<CPlayerController>()->SetActive(false);
			playerTwo.GetGameObject().GetComponent<CStateMachine>()->SetState(IdleState::Name());

			playerOne.GetGameObject().Transform().SetLocalPositionX(initPosOne.x);
			playerOne.GetGameObject().Transform().SetLocalPositionY(initPosOne.y);

			playerTwo.GetGameObject().Transform().SetLocalPositionX(initPosTwo.x);
			playerTwo.GetGameObject().Transform().SetLocalPositionY(initPosTwo.y);
		};

	auto enableInput = [playerOne, playerTwo, initPosOne, initPosTwo]() mutable
		{
			playerOne.GetGameObject().GetComponent<CPlayerController>()->SetActive(true);
			playerTwo.GetGameObject().GetComponent<CPlayerController>()->SetActive(true);
		};

	auto changeToHighscoreScene = [highscoreSceneName]() {SceneManager::GetInstance().ChangeCurrentScene(highscoreSceneName); };
	

	std::vector<Player> players{ playerOne , playerTwo};

	//----- Name Assigner Scene

	NameAssignerSceneData data{};

	for (size_t i = 0; i < players.size(); i++)
	{
		data.GameObjectsToAssignName.push_back(players[i].GetGameObjectHandle());
	}

	data.SceneName = GetNameAssignerSceneName();
	data.SceneNameUponCompletion = PlayerVsScene::GetSceneName();

	NameAssignerScene nameAssignerScene{ data };

	
	auto sceneCreationFunction = [players, disableInputAndReset, enableInput, changeToHighscoreScene](Scene& scene) mutable{


		InputControllerData inputData{};
		inputData.AllowKeyboard = true;
		inputData.HowToTreatKeyboard = players.size() > 1 ? HowToTreatKeyboard::MakeItAnIndependentPlayerId : HowToTreatKeyboard::SharePlayerIdWithFirstController;
		inputData.MaxControllers = players.size() > XUSER_MAX_COUNT ? XUSER_MAX_COUNT : players.size();

		InputManager::GetFromScene(&scene).SetData(inputData);

		auto particleSystem = CreateParticleSystem();
		scene.AddGameObjectHandle(particleSystem);

		for (size_t i = 0; i < players.size(); i++)
		{
			players[i].SubscribeOnPlayerDespawnFromDamage(disableInputAndReset);//disable input and reset pos
			players[i].SubscribeOnPlayerRespawnFromDamage(enableInput);//enable input and reset pos
			players[i].SubscribeOnPlayerDie(changeToHighscoreScene);
			players[i].GetGameObject().SetActive(true);
			players[i].AddScene(scene);
		}
		
		};

	SceneManager::GetInstance().AddScene(this->GetSceneName(), sceneCreationFunction);


	auto highscoreSceneCreationFunction = [players](Scene& scene) mutable 
		{


			InputControllerData inputData{};
			inputData.AllowKeyboard = true;
			inputData.HowToTreatKeyboard = players.size() > 1 ? HowToTreatKeyboard::MakeItAnIndependentPlayerId : HowToTreatKeyboard::SharePlayerIdWithFirstController;
			inputData.MaxControllers = players.size() > XUSER_MAX_COUNT ? XUSER_MAX_COUNT : players.size();

			InputManager::GetFromScene(&scene).SetData(inputData);


			auto particleSystem = CreateParticleSystem();
			scene.AddGameObjectHandle(particleSystem);

			int maxHealth = -1;

			std::string whoWon{};

			for (size_t i = 0; i < players.size(); i++)
			{
				int currentHealth = players[i].GetGameObject().GetComponent<CStatController>()->GetStat(StatType::Health);
				if (maxHealth < currentHealth)
				{
					maxHealth = currentHealth;
					whoWon = players[i].GetGameObject().GetName();
				}
				else if (maxHealth == currentHealth)
				{

					whoWon += " and " + players[i].GetGameObject().GetName();
				}
			}
			TextCreator textCreator{ "WINNER: " + whoWon, {g_WindowWidth / 2.f, g_WindowHeight / 2.f}, 15, {255, 255, 255} };
			scene.AddGameObjectHandle(textCreator.GetGameObjectHandle());


			auto backToMainMenu = [players](GameObject&) mutable
				{
					for (size_t i = 0; i < players.size(); i++)
					{
						players[i].GetGameObject().GetComponent<CStatController>()->SetStat(StatType::NumberOfHits, 0);
						players[i].GetGameObject().GetComponent<CStatController>()->SetStat(StatType::Points, 0);
						players[i].GetGameObject().GetComponent<CStatController>()->SetStat(StatType::ShotsFired, 0);
						players[i].GetGameObject().GetComponent<CStatController>()->ResetCurrentStat(StatType::Health);

					}
					SceneManager::GetInstance().ChangeCurrentScene(MainMenu::Name());
				};

			Event<GameObject&> backToMainMenuEvent{};
			backToMainMenuEvent.Subscribe(backToMainMenu);

			for (size_t i = 0; i < players.size(); i++)
			{
				players[i].GetGameObject().SetActive(true, false);
				players[i].GetGameObject().GetComponent<CCollider>()->SetActive(false);
				players[i].AddScene(scene); 
				CPlayerController& playerController = *players[i].GetGameObject().GetComponent<CPlayerController>();

				playerController.BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_RETURN,				std::make_shared<EventTriggerCommand>(backToMainMenuEvent) });
				playerController.BindKey(dae::PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::RightShoulder,		std::make_shared<EventTriggerCommand>(backToMainMenuEvent) });

			}

			TextCreator message("PRESS RETURN OR RIGHT SHOULDER TO EXIT", { int(g_WindowWidth / 2.f),   int(g_WindowHeight * 5.5 / 6.f) }, 13, SDL_Color{ 255, 255 , 255 });
			scene.AddGameObjectHandle(message.GetGameObjectHandle());

		};

	SceneManager::GetInstance().AddScene(highscoreSceneName, highscoreSceneCreationFunction);
}
