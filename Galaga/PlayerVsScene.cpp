#include "PlayerVsScene.h"
#include "Player.h"

dae::PlayerVsScene::PlayerVsScene()
{
	//
	auto sceneCreationFunction = [](Scene& scene) {
		
		GalagaPlayer playerOne{ glm::vec2{} , 180 };
		GalagaPlayer playerTwo{ glm::vec2{} , 180 };

		
		};

	SceneManager::GetInstance().AddScene(this->GetSceneName(), sceneCreationFunction);
}
