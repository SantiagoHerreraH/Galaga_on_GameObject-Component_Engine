#pragma once

#include "Scene.h"

namespace dae {

	class PlayerVsScene {

	public:
		PlayerVsScene();

		const std::string GetNameAssignerSceneName() { return "NameAssignerScene_Versus"; }
		const std::string GetSceneName() { return "PlayerVERSUSScene"; }
		Scene& GetScene() { return *SceneManager::GetInstance().GetScene(GetSceneName()); }

	};
}