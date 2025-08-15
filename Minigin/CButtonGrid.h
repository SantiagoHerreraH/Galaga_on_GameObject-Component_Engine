#pragma once

#include <vector>
#include <glm.hpp>
#include "EventSystem.h"
#include "Button.h"
#include "PlayerController.h"

namespace dae {

	struct ButtonGridData {
		glm::ivec2 StartPos;
		int ColumnNumber; // row num is expandable 
		int OffsetBetweenRows;
		int OffsetBetweenCols;
	};

	class CPlayerController;

	class CButtonGrid final : public Component {

	public:

		CButtonGrid(const ButtonGridData& buttonGridData);

		void Start()override;

		bool AddButton(GameObjectHandle button);

	private:
		CPlayerController* GetPlayerController();
		void MapButtonSelectionToKeys();

		struct ButtonGridInternalData {
			std::vector<GameObjectHandle> ButtonGameObjs;
			std::vector<CButton*> Buttons;
		};

		struct ButtonGridEvents {
			dae::Event<GameObject&> OnMoveUp;
			dae::Event<GameObject&> OnMoveDown;
			dae::Event<GameObject&> OnMoveLeft;
			dae::Event<GameObject&> OnMoveRight;
			dae::Event<GameObject&> OnPress;
		};


		ButtonGridEvents m_ButtonGridEvents{};
		ButtonGridData m_ButtonGridData{};
		int m_CurrentColIdx{0};
		int m_CurrentRowIdx{0};

		std::shared_ptr<ButtonGridInternalData> m_ButtonGridInternalData;

	};
}