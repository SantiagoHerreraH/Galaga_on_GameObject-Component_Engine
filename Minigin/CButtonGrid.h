#pragma once

#include <vector>
#include <glm.hpp>
#include "EventSystem.h"
#include "Button.h"

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

		bool AddButton(CButton& button);
		bool AddButton(GameObject& button);

	private:
		CPlayerController* GetPlayerController();
		void MapButtonSelectionToKeys();


		struct ButtonGridInternalData {
			std::vector<CButton*> Buttons;
		};

		struct ButtonGridEvents {
			dae::Event<> OnMoveUp;
			dae::Event<> OnMoveDown;
			dae::Event<> OnMoveLeft;
			dae::Event<> OnMoveRight;
			dae::Event<> OnPress;
		};


		ButtonGridEvents m_ButtonGridEvents;
		ButtonGridData m_ButtonGridData;
		int m_CurrentColIdx;
		int m_CurrentRowIdx;

		std::shared_ptr<ButtonGridInternalData> m_ButtonGridInternalData;

	};
}