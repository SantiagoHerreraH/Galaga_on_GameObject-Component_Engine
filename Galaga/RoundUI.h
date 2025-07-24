#pragma once

#include "Scene.h"
#include "Text.h"

#include "Settings.h"

namespace dae {

	class RoundUI {

	public:
		RoundUI(dae::Scene& scene, int roundNumber);

		void SubscribeOnRoundTextEnd(const std::function<void()>& onRoundTextEnd);

	private:

		GameObjectHandle m_RoundNumBeginning;
		GameObjectHandle m_RoundNum;

		GameObjectHandle CreateRoundNum(dae::Scene& scene, int roundNumber, const glm::vec2& startPos);

		GameObjectHandle CreateRoundNumBeginning(dae::Scene& scene, int roundNumber);
	};

	
}