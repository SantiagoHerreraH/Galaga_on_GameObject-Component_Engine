#include "PlayerController.h"

dae::PlayerController::PlayerController()
{
	m_IsValid = InputManager::GetInstance().GetNextAvailableControllerInstance(m_ControllerInstance);
}

void dae::PlayerController::Start()
{
	if (m_IsValid)
	{
		GamepadKeyData gamePagKeyData{};
		gamePagKeyData.PlayerId = m_ControllerInstance.PlayerId;
		gamePagKeyData.PlayerGameObject = &Owner();

		for (size_t i = 0; i < m_PlayerGamepadKeyData.size(); i++)
		{
			gamePagKeyData.ButtonState   = m_PlayerGamepadKeyData[i].ButtonState   ;
			gamePagKeyData.GamepadButton = m_PlayerGamepadKeyData[i].GamepadButton ;
			gamePagKeyData.OnTriggered   = std::move(m_PlayerGamepadKeyData[i].OnTriggered);
			InputManager::GetInstance().BindKey(gamePagKeyData);
		}

		KeyboardKeyData keyboardKeyData{};
		keyboardKeyData.PlayerGameObject = &Owner();

		for (size_t i = 0; i < m_PlayerKeyboardKeyData.size(); i++)
		{
			keyboardKeyData.ButtonState	 = m_PlayerKeyboardKeyData[i].ButtonState;
			keyboardKeyData.Key			 = m_PlayerKeyboardKeyData[i].Key			;
			keyboardKeyData.OnTriggered	 = std::move(m_PlayerKeyboardKeyData[i].OnTriggered);

			InputManager::GetInstance().BindKey(keyboardKeyData);
		}
	}
}

bool dae::PlayerController::IsValid() const
{
	return m_IsValid;
}

dae::PlayerId dae::PlayerController::GetPlayerId()
{
	return m_ControllerInstance.PlayerId;
}

dae::ControllerType dae::PlayerController::GetControllerType()
{
	return m_ControllerInstance.ControllerType;
}

bool dae::PlayerController::BindKey(PlayerGamepadKeyData playerGamepadKeyData)
{
	if (m_IsValid && m_ControllerInstance.ControllerType != ControllerType::Keyboard)
	{
		if (HasOwner())
		{
			GamepadKeyData gamePagKeyData{};
			gamePagKeyData.PlayerId = m_ControllerInstance.PlayerId;
			gamePagKeyData.PlayerGameObject = &Owner();

			gamePagKeyData.ButtonState = playerGamepadKeyData.ButtonState;
			gamePagKeyData.GamepadButton = playerGamepadKeyData.GamepadButton;
			gamePagKeyData.OnTriggered = std::move(playerGamepadKeyData.OnTriggered);
			InputManager::GetInstance().BindKey(gamePagKeyData);
		}
		else
		{
			m_PlayerGamepadKeyData.push_back(std::move(playerGamepadKeyData));
		}

		return true;
	}

	return false;
}

bool dae::PlayerController::BindKey(PlayerKeyboardKeyData playerKeyboardKeyData)
{
	if (m_IsValid && m_ControllerInstance.ControllerType != ControllerType::Gamepad)
	{
		if (HasOwner())
		{
			KeyboardKeyData gamePagKeyData{};
			gamePagKeyData.PlayerGameObject = &Owner();
			gamePagKeyData.ButtonState = playerKeyboardKeyData.ButtonState;
			gamePagKeyData.Key = playerKeyboardKeyData.Key;
			gamePagKeyData.OnTriggered = std::move(playerKeyboardKeyData.OnTriggered);
			InputManager::GetInstance().BindKey(gamePagKeyData);
		}
		else
		{
			m_PlayerKeyboardKeyData.push_back(std::move(playerKeyboardKeyData));
		}

		return true;
	}

	return false;
}
