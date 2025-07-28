#include "PlayerVsScene.h"
#include "Player.h"
#include "Gun.h"
#include "CaptureZoneWeaponType.h"
#include "Settings.h"
#include "PlayerController.h"
#include "HighscoreScene.h"

dae::PlayerVsScene::PlayerVsScene()
{
	int xPos = g_WindowWidth / 2.f;
	int yOffset = 50;
	const std::string highscoreSceneName = "PlayerVsHighscoreScene";

	//---Player One
	PlayerType playerType{};
	playerType.TextureName = "galaga.png";
	playerType.WeaponType = std::make_shared<GunWeaponType>();

	glm::vec2 initPosOne{ xPos, g_WindowHeight - yOffset };
	Player playerOne{ initPosOne, 180, playerType };

	//---Player Two
	playerType.TextureName = "boss.png";
	playerType.WeaponType = std::make_shared<CaptureZoneWeaponType>(playerOne.GetGameObjectHandle());

	glm::vec2 initPosTwo{ xPos, yOffset };
	Player playerTwo{ initPosTwo, 0, playerType };

	//---Functions
	auto disableInputAndReset = [playerOne, playerTwo, initPosOne, initPosTwo]() mutable
		{
			playerOne.GetGameObject().GetComponent<CPlayerController>()->SetActive(false);
			playerTwo.GetGameObject().GetComponent<CPlayerController>()->SetActive(false);

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

	//---Set Up
	playerOne.SubscribeOnPlayerDespawnFromDamage(disableInputAndReset);//disable input and reset pos
	playerOne.SubscribeOnPlayerRespawnFromDamage(enableInput);//enable input and reset pos

	playerTwo.SubscribeOnPlayerDespawnFromDamage(disableInputAndReset);//disable input and reset pos
	playerTwo.SubscribeOnPlayerRespawnFromDamage(enableInput);//enable input and reset pos

	playerOne.SubscribeOnPlayerDie(changeToHighscoreScene);
	playerOne.SubscribeOnPlayerDie(changeToHighscoreScene);

	//---HighscoreScene
	std::vector<Player> players{ playerOne , playerTwo};

	HighscoreScene highscoreScene{ players,  highscoreSceneName };
	
	auto sceneCreationFunction = [players](Scene& scene) mutable{

		for (size_t i = 0; i < players.size(); i++)
		{
			players[i].AddScene(scene);
		}
		
		};

	SceneManager::GetInstance().AddScene(this->GetSceneName(), sceneCreationFunction);
}
