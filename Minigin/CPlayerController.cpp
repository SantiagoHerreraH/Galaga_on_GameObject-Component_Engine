#include "CPlayerController.h"

dae::CPlayerController::CPlayerController()
{
}

void dae::CPlayerController::Start()
{
	m_IsValid = InputManager::GetCurrent().GetNextAvailableControllerInstance(m_ControllerInstance);

	if (m_IsValid)
	{
		if (m_ControllerInstance.ControllerType != ControllerType::Keyboard)
		{
			GamepadKeyData gamePadKeyData{};
			gamePadKeyData.PlayerId = m_ControllerInstance.PlayerId;
			gamePadKeyData.PlayerGameObject = &Owner();

			for (size_t i = 0; i < m_PlayerGamepadKeyData.size(); i++)
			{
				gamePadKeyData.ButtonState = m_PlayerGamepadKeyData[i].ButtonState;
				gamePadKeyData.GamepadButton = m_PlayerGamepadKeyData[i].GamepadButton;
				gamePadKeyData.OnTriggered = m_PlayerGamepadKeyData[i].OnTriggered;
				InputManager::GetCurrent().BindKey(gamePadKeyData);
			}
		}
		if (m_ControllerInstance.ControllerType != ControllerType::Gamepad)
		{
			KeyboardKeyData keyboardKeyData{};
			keyboardKeyData.PlayerGameObject = &Owner();

			for (size_t i = 0; i < m_PlayerKeyboardKeyData.size(); i++)
			{
				keyboardKeyData.ButtonState = m_PlayerKeyboardKeyData[i].ButtonState;
				keyboardKeyData.Key = m_PlayerKeyboardKeyData[i].Key;
				keyboardKeyData.OnTriggered = m_PlayerKeyboardKeyData[i].OnTriggered;

				InputManager::GetCurrent().BindKey(keyboardKeyData);
			}
		}

		
	}
}

void dae::CPlayerController::SetActive(bool isActive)
{
	Component::SetActive(isActive);

	if (!m_IsValid)
	{
		return;
	}
	if (isActive)
	{
		InputManager::GetCurrent().EnableInput(m_ControllerInstance.PlayerId);
	}
	else
	{
		InputManager::GetCurrent().DisableInput(m_ControllerInstance.PlayerId);
	}
}

bool dae::CPlayerController::IsValid() const
{
	return m_IsValid;
}

dae::ControllerType dae::CPlayerController::GetControllerType()
{
	return m_ControllerInstance.ControllerType;
}

bool dae::CPlayerController::BindKey(PlayerGamepadKeyData playerGamepadKeyData)
{
	if (m_ControllerInstance.ControllerType != ControllerType::Keyboard)
	{
		if (HasOwner() && m_IsValid)
		{
			GamepadKeyData gamePagKeyData{};
			gamePagKeyData.PlayerId = m_ControllerInstance.PlayerId;
			gamePagKeyData.PlayerGameObject = &Owner();

			gamePagKeyData.ButtonState = playerGamepadKeyData.ButtonState;
			gamePagKeyData.GamepadButton = playerGamepadKeyData.GamepadButton;
			gamePagKeyData.OnTriggered = std::move(playerGamepadKeyData.OnTriggered);
			InputManager::GetCurrent().BindKey(gamePagKeyData);
		}
		else
		{
			m_PlayerGamepadKeyData.push_back(playerGamepadKeyData);
		}

		return true;
	}

	return false;
}

bool dae::CPlayerController::BindKey(PlayerKeyboardKeyData playerKeyboardKeyData)
{
	if (m_ControllerInstance.ControllerType != ControllerType::Gamepad)
	{
		if (HasOwner() && m_IsValid)
		{
			KeyboardKeyData gamePagKeyData{};
			gamePagKeyData.PlayerGameObject = &Owner();
			gamePagKeyData.ButtonState = playerKeyboardKeyData.ButtonState;
			gamePagKeyData.Key = playerKeyboardKeyData.Key;
			gamePagKeyData.OnTriggered = std::move(playerKeyboardKeyData.OnTriggered);
			InputManager::GetCurrent().BindKey(gamePagKeyData);
		}
		else
		{
			m_PlayerKeyboardKeyData.push_back(playerKeyboardKeyData);
		}

		return true;
	}

	return false;
}
