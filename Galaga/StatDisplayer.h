#pragma once

#include "Scene.h"
#include "Text.h"
#include "InputManager.h"

#include "Settings.h"
#include "Component.h"
#include "GalagaStats.h"

namespace dae {

	struct StatDisplayData {
		
		TextData StatNameTextData;
		StatType StatTypeToDisplay;
		SDL_Color StatValueColor;

		glm::vec2 Where;
		glm::vec2 StatValueOffsetFromStatName;
		float StatValueBaseOffsetMultiplierX{1}; // 1 == exaclty to the right
		float StatValueBaseOffsetMultiplierY{1}; // 1 == exaclty below

		GameObjectHandle FromWho;
		bool UpdateAutomatically{ true };
	};

	class CStatDisplayer : public Component {

	public:

		CStatDisplayer(const StatDisplayData& statDisplayData);//if from who == nullptr -> from Owner()
		void Start()override;
		void StopDisplaying();
		void StartDisplaying();

	private:

		bool m_Subscribed;
		StatDisplayData m_StatDisplayData;
		GameObjectHandle m_StatName;
		GameObjectHandle m_StatValue;
	};


}