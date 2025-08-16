#include "Scene_PlayerVs.h"
#include "Scene_Highscore.h"
#include "Scene_MainMenu.h"
#include "CGun.h"
#include "WeaponType_CaptureZone.h"
#include "WeaponType_Gun.h"
#include "Settings.h"
#include "CPlayerController.h"
#include "Create_ParticleSystem.h"
#include "Scene_NameAssigner.h"
#include "CCaptureZone.h"
#include "TextCreator.h"
#include "CGameStatController.h"
#include "CCollider.h"
#include "EventTriggerCommand.h"

dae::Scene_PlayerVs::Scene_PlayerVs()
{
	SceneData_PlayerVs playerVsSceneData{};

	int xPos = g_WindowWidth / 2.f;
	int yOffset = 50;

	//---Player One
	PlayerType playerType{};
	playerType.HasVerticalMovement = false;
	playerType.PlayerCollisionLayer = CollisionLayers::Player;
	playerType.TextureName = "galaga.png";
	playerType.TextureScale = 0.5f;
	playerType.WeaponType = std::make_shared<WeaponType_Gun>(CollisionLayers::PlayerBullets, CollisionLayers::Enemies);

	playerVsSceneData.InitPosOne = { xPos, g_WindowHeight - yOffset };
	playerVsSceneData.PlayerOne = std::move(Player{ playerVsSceneData.InitPosOne, 0, 0, playerType });

	//---Player Two

	playerType.HasVerticalMovement = false;
	playerType.PlayerCollisionLayer = CollisionLayers::Enemies;
	playerType.TextureName = "BossVs.png";
	playerType.TextureScale = 2.f;
	float bossPlayerSpeed = 350;
	playerType.WeaponType = std::make_shared<WeaponType_CaptureZone>(playerVsSceneData.PlayerOne.GetGameObjectHandle(), bossPlayerSpeed);

	playerVsSceneData.InitPosTwo = { xPos, yOffset };
	playerVsSceneData.PlayerTwo  = std::move(Player{ playerVsSceneData.InitPosTwo, 0, 1, playerType });

	CreatePlayerVSScene(playerVsSceneData);
	CreateHighscoreScene({ playerVsSceneData.PlayerOne , playerVsSceneData.PlayerTwo });

}



void dae::Scene_PlayerVs::CreatePlayerVSScene(SceneData_PlayerVs playerVsSceneData)
{

	auto disableInputAndReset = [playerVsSceneData]() mutable
		{
			playerVsSceneData.PlayerOne.GetGameObject().GetComponent<CPlayerController>()->SetActive(false);
			playerVsSceneData.PlayerTwo.GetGameObject().GetComponent<CPlayerController>()->SetActive(false);
			playerVsSceneData.PlayerTwo.GetGameObject().GetComponent<CStateMachine>()->SetState(IdleState::Name());
			
			playerVsSceneData.PlayerOne.GetGameObject().Transform().SetLocalPositionX(playerVsSceneData.InitPosOne.x);
			playerVsSceneData.PlayerOne.GetGameObject().Transform().SetLocalPositionY(playerVsSceneData.InitPosOne.y);
			
			playerVsSceneData.PlayerTwo.GetGameObject().Transform().SetLocalPositionX(playerVsSceneData.InitPosTwo.x);
			playerVsSceneData.PlayerTwo.GetGameObject().Transform().SetLocalPositionY(playerVsSceneData.InitPosTwo.y);
		};

	auto enableInput = [playerVsSceneData]() mutable
		{
			playerVsSceneData.PlayerOne.GetGameObject().GetComponent<CPlayerController>()->SetActive(true);
			playerVsSceneData.PlayerTwo.GetGameObject().GetComponent<CPlayerController>()->SetActive(true);
		};

	auto changeToHighscoreScene = []() {SceneManager::GetInstance().ChangeCurrentScene(Scene_PlayerVs::GetHighscoreSceneName()); };


	std::vector<Player> players{ playerVsSceneData.PlayerOne , playerVsSceneData.PlayerTwo };

	//----- Name Assigner Scene

	SceneData_NameAssigner data{};

	for (size_t i = 0; i < players.size(); i++)
	{
		data.GameObjectsToAssignName.push_back(players[i].GetGameObjectHandle());
	}

	data.SceneName = GetNameAssignerSceneName();
	data.SceneNameUponCompletion = Scene_PlayerVs::GetSceneName();

	Scene_NameAssigner nameAssignerScene{ data };


	auto sceneCreationFunction = [players, disableInputAndReset, enableInput, changeToHighscoreScene](Scene& scene) mutable {


		InputControllerData inputData{};
		inputData.AllowKeyboard = true;
		inputData.HowToTreatKeyboard = players.size() > 1 ? HowToTreatKeyboard::MakeItAnIndependentPlayerId : HowToTreatKeyboard::SharePlayerIdWithFirstController;
		inputData.MaxControllers = players.size() > XUSER_MAX_COUNT ? XUSER_MAX_COUNT : players.size();

		InputManager::GetFromScene(&scene).SetData(inputData);

		auto particleSystem = Create_ParticleSystem();
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
}

void dae::Scene_PlayerVs::CreateHighscoreScene(std::vector<Player> players) 
{
	
	auto highscoreSceneCreationFunction = [players](Scene& scene) mutable
		{


			InputControllerData inputData{};
			inputData.AllowKeyboard = true;
			inputData.HowToTreatKeyboard = players.size() > 1 ? HowToTreatKeyboard::MakeItAnIndependentPlayerId : HowToTreatKeyboard::SharePlayerIdWithFirstController;
			inputData.MaxControllers = players.size() > XUSER_MAX_COUNT ? XUSER_MAX_COUNT : players.size();

			InputManager::GetFromScene(&scene).SetData(inputData);


			auto particleSystem = Create_ParticleSystem();
			scene.AddGameObjectHandle(particleSystem);

			int maxHealth = -1;

			std::string whoWon{};

			for (size_t i = 0; i < players.size(); i++)
			{
				int currentHealth = players[i].GetGameObject().GetComponent<CGameStatController>()->GetStat(GameStatType::Health);
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
						players[i].GetGameObject().GetComponent<CGameStatController>()->SetStat(GameStatType::NumberOfHits, 0);
						players[i].GetGameObject().GetComponent<CGameStatController>()->SetStat(GameStatType::Points, 0);
						players[i].GetGameObject().GetComponent<CGameStatController>()->SetStat(GameStatType::ShotsFired, 0);
						players[i].GetGameObject().GetComponent<CGameStatController>()->ResetCurrentStat(GameStatType::Health);

					}
					SceneManager::GetInstance().ChangeCurrentScene(Scene_MainMenu::Name());
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

	SceneManager::GetInstance().AddScene(Scene_PlayerVs::GetHighscoreSceneName(), highscoreSceneCreationFunction);
}