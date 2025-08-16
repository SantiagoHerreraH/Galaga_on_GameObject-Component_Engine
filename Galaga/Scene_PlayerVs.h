#pragma once

#include "Scene.h"
#include "Player.h"

namespace dae {


	class Scene_PlayerVs {

	public:
		Scene_PlayerVs();

		static const std::string& GetNameAssignerSceneName()
		{ 
			static std::string name{ "NameAssignerScene_Versus" };
			return name;
		}
		static const std::string& GetSceneName() 
		{
			static std::string name{ "PlayerVERSUSScene" };
			return name;
		}
		static const std::string& GetHighscoreSceneName()
		{
			static std::string name{ "PlayerVsHighscoreSceneName" };
			return name;
		}

		Scene& GetScene() { return *SceneManager::GetInstance().GetScene(GetSceneName()); }

	private:

		struct SceneData_PlayerVs {

			Player PlayerOne{};
			glm::vec2 InitPosOne{};

			Player PlayerTwo{};
			glm::vec2 InitPosTwo{};

		};

		void CreatePlayerVSScene(SceneData_PlayerVs playerVsSceneData);
		void CreateHighscoreScene(std::vector<Player> players);
	};
}