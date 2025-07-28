#pragma once

#include "Scene.h"

namespace dae {

	class PlayerVsScene {

	public:
		PlayerVsScene();
		const std::string GetSceneName() { return "VERSUS"; }
		Scene& GetScene() { return *SceneManager::GetInstance().GetScene(GetSceneName()); }

	};
}