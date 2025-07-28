#include "PlayerVsScene.h"
#include "Player.h"
#include "Gun.h"
#include "CaptureZoneWeaponType.h"
#include "Settings.h"
#include "PlayerController.h"

dae::PlayerVsScene::PlayerVsScene()
{
	//
	auto sceneCreationFunction = [](Scene& scene) {
		
		int xPos = g_WindowWidth/2.f;
		int yOffset = 50;

		PlayerType playerType{};
		playerType.TextureName = "galaga.png";
		playerType.WeaponType = std::make_shared<GunWeaponType>();

		glm::vec2 initPosOne{ xPos, g_WindowHeight - yOffset };
		GalagaPlayer playerOne{ initPosOne, 180, playerType };

		playerType.TextureName = "boss.png";
		playerType.WeaponType = std::make_shared<CaptureZoneWeaponType>(playerOne.GetGameObjectHandle());


		glm::vec2 initPosTwo{ xPos, yOffset };
		GalagaPlayer playerTwo{ initPosTwo, 0, playerType };

		playerOne.AddScene(scene);
		playerTwo.AddScene(scene);

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

		playerOne.SubscribeOnPlayerDespawnFromDamage(disableInputAndReset);//disable input and reset pos
		playerOne.SubscribeOnPlayerRespawnFromDamage(enableInput);//enable input and reset pos

		playerOne.SubscribeOnPlayerDie();//go to highscore scene


		
		};

	SceneManager::GetInstance().AddScene(this->GetSceneName(), sceneCreationFunction);
}
