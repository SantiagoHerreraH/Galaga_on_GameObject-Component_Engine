#pragma once

#include "Scene.h"
#include <SDL.h>
#include "CButtonGrid.h"

namespace dae {


	class MainMenu {

	public:

		MainMenu();

		const static std::string& Name() {
			static std::string name{ "MainMenu" };
			return name;
		}

		void AddSceneChangeButton(const std::string& sceneName, const std::string& buttonName = {});
	private:

		class MainMenuCreator {
		public:
			MainMenuCreator();
			void CreateParticles(dae::Scene& scene);
			void CreateTexture(dae::Scene& scene, const std::string& fileName, const glm::vec2& pos, const glm::vec2& scale = { 1,1 });
			void CreateTitleAndSubTitle(dae::Scene& scene, const std::string& title, const std::string& subTitle, const glm::vec2& pos);

			void CreateButton(const std::string& buttonName, std::string sceneNameToTransition);
			void AddGridToScene(dae::Scene& scene);

		private:
			void CreateGrid();
			dae::CButtonGrid* GetGrid();
			GameObjectHandle m_ButtonGrid;
		};

		std::shared_ptr<MainMenuCreator> m_MainMenuCreator{};
	};
	
}