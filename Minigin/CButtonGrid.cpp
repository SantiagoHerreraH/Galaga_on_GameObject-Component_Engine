#include "CButtonGrid.h"
#include "GameObject.h"
#include "InputManager.h"
#include "CPlayerController.h"
#include "EventTriggerCommand.h"

dae::CButtonGrid::CButtonGrid(const ButtonGridData& buttonGridData) :
	m_ButtonGridData(buttonGridData),
	m_ButtonGridInternalData(std::make_shared<ButtonGridInternalData>())
{
	const int colNum = m_ButtonGridData.ColumnNumber;

	auto data = m_ButtonGridInternalData;


	std::shared_ptr<int> currentIdx{ std::make_shared<int>(0) };

	m_ButtonGridEvents.OnMoveUp.Subscribe([data, currentIdx, colNum](GameObject&) mutable
		{

			data->Buttons[(*currentIdx)]->Unselect();

			int row = (*currentIdx) / colNum;
			int col = (*currentIdx) % colNum;

			if (row > 0)
				--row;

			int newIdx = row * colNum + col;
			if (newIdx < data->Buttons.size())
				(*currentIdx) = newIdx;


			data->Buttons[(*currentIdx)]->Select();
		});


	m_ButtonGridEvents.OnMoveDown.Subscribe([data, currentIdx, colNum](GameObject&) mutable {

		data->Buttons[(*currentIdx)]->Unselect();
		int row = (*currentIdx) / colNum;
		int col = (*currentIdx) % colNum;

		++row;
		int newIdx = row * colNum + col;

		if (newIdx < data->Buttons.size())
			(*currentIdx) = newIdx;



		data->Buttons[(*currentIdx)]->Select();
		});

	m_ButtonGridEvents.OnMoveLeft.Subscribe([data, currentIdx, colNum](GameObject&) mutable {

		data->Buttons[(*currentIdx)]->Unselect();
		int row = (*currentIdx) / colNum;
		int col = (*currentIdx) % colNum;

		if (col > 0)
			--col;
		else
			col = 0; // optional: stay on first column if already at leftmost

		int newIdx = row * colNum + col;
		if (newIdx < data->Buttons.size())
			(*currentIdx) = newIdx;


		data->Buttons[(*currentIdx)]->Select();
		});

	m_ButtonGridEvents.OnMoveRight.Subscribe([data, currentIdx, colNum](GameObject&) mutable {


		data->Buttons[(*currentIdx)]->Unselect();
		int row = (*currentIdx) / colNum;
		int col = (*currentIdx) % colNum;

		++col;

		int newIdx = row * colNum + col;
		if (col < colNum && newIdx < data->Buttons.size())
			(*currentIdx) = newIdx;

		data->Buttons[(*currentIdx)]->Select();

		});

	m_ButtonGridEvents.OnPress.Subscribe([data, currentIdx](GameObject&) mutable {
		data->Buttons[*currentIdx]->Press();
		});

}

void dae::CButtonGrid::Start()
{
	MapButtonSelectionToKeys();

	for (size_t i = 0; i < m_ButtonGridInternalData->ButtonGameObjs.size(); i++)
	{
		SceneManager::GetInstance().GetCurrentScene().AddGameObjectHandle(m_ButtonGridInternalData->ButtonGameObjs[i]);
	}
}

bool dae::CButtonGrid::AddButton(GameObjectHandle button)
{
	CButton* buttonComp = button->GetComponent<CButton>();
	if (buttonComp)
	{
		glm::vec2 pos{
			   m_ButtonGridData.StartPos.x + (m_ButtonGridData.OffsetBetweenCols * m_CurrentColIdx),
			   m_ButtonGridData.StartPos.y + (m_ButtonGridData.OffsetBetweenRows * m_CurrentRowIdx) };

		button->Transform().SetLocalPositionX(pos.x);
		button->Transform().SetLocalPositionY(pos.y);

		if (m_ButtonGridInternalData->Buttons.empty())
		{
			buttonComp->Select();
		}

		m_ButtonGridInternalData->ButtonGameObjs.push_back(button);
		m_ButtonGridInternalData->Buttons.push_back(buttonComp);

		++m_CurrentColIdx;

		if (m_CurrentColIdx >= m_ButtonGridData.ColumnNumber)
		{
			m_CurrentColIdx = 0;

			++m_CurrentRowIdx;

		}

		return true;
	}

	return false;
}


dae::CPlayerController* dae::CButtonGrid::GetPlayerController() {

	CPlayerController* playerController = Owner().GetComponent<CPlayerController>();
	if (!playerController)
	{
		Owner().AddComponent(CPlayerController{});
		playerController = Owner().GetComponent<CPlayerController>();
	}
	return playerController;
}

void dae::CButtonGrid::MapButtonSelectionToKeys()
{	

	CPlayerController* playerController = GetPlayerController();

	playerController->BindKey(PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::ButtonX,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnPress) });
	playerController->BindKey(PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::DpadLeft,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveLeft) });
	playerController->BindKey(PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::DpadRight,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveRight) });
	playerController->BindKey(PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::DpadUp,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveUp) });
	playerController->BindKey(PlayerGamepadKeyData{ ButtonState::BUTTON_DOWN, GamepadButton::DpadDown,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveDown) });

	playerController->BindKey(PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_SPACE,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnPress) });
	playerController->BindKey(PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_LEFT,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveLeft) });
	playerController->BindKey(PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_RIGHT,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveRight) });
	playerController->BindKey(PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_UP,			std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveUp) });
	playerController->BindKey(PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_DOWN,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveDown) });

	playerController->BindKey(PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_A,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveLeft) });
	playerController->BindKey(PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_D,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveRight) });
	playerController->BindKey(PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_W,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveUp) });
	playerController->BindKey(PlayerKeyboardKeyData{ ButtonState::BUTTON_DOWN, SDL_SCANCODE_S,		std::make_shared<EventTriggerCommand>(m_ButtonGridEvents.OnMoveDown) });
}