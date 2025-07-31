#pragma once
#include <string>
#include <vector>
#include "Player.h"

namespace dae {

	class HighscoreScene {

	public:

		HighscoreScene(std::vector<Player> players, const std::string& sceneName, std::string gameModeName);
		const std::string& GetName()const { return m_SceneName; }
		void SubscribeOnHighscoreSceneEnd(const std::function<void()>& func);

	private:
		std::string m_SceneName;
		std::shared_ptr<Event<>> m_OnHighscoreEnd;


	};
}