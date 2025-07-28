#pragma once
#include <string>
#include <vector>
#include "Player.h"

namespace dae {

	class HighscoreScene {

	public:

		HighscoreScene(std::vector<GalagaPlayer> players, const std::string& sceneName);
		const std::string& GetName()const { return m_SceneName; }

	private:
		std::string m_SceneName;

	};
}