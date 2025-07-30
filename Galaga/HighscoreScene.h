#pragma once
#include <string>
#include <vector>
#include "Player.h"

namespace dae {

	class HighscoreScene {

	public:

		HighscoreScene(std::vector<Player> players, const std::string& sceneName, std::string gameModeName);
		const std::string& GetName()const { return m_SceneName; }

	private:
		std::string m_SceneName;

	};
}