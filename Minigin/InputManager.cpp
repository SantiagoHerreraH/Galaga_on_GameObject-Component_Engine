#include <SDL.h>
#include "InputManager.h"
//#include "imgui_impl_sdl2.h"
//#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <Xinput.h>
#include <glm.hpp>
#include <iostream>

void dae::InputManager::ClearKeys()
{
	m_GamepadEvents.clear();
	m_KeyboardEvents.clear();
}

void dae::InputManager::BindKey(const GamepadKeyData& gamepadKeyData)
{
	m_GamepadEvents.push_back(gamepadKeyData);
}

void dae::InputManager::BindKey(const KeyboardKeyData& keyboardKeyData)
{
	m_KeyboardEvents.push_back(keyboardKeyData);
}

#pragma region InputManager


dae::InputManager::InputManager(const InputControllerData& defaultData) :
	m_Data(defaultData)
{
	m_KeyboardKeyStates.resize(SDL_NUM_SCANCODES);
	m_Data.MaxControllers = m_Data.MaxControllers > XUSER_MAX_COUNT ?  XUSER_MAX_COUNT : m_Data.MaxControllers;
	m_GamepadInputManagers.reserve(m_Data.MaxControllers);

	for (size_t i = 0; i < m_Data.MaxControllers; i++)
	{
		m_GamepadInputManagers.push_back(GamepadInputManager(i)); // Add to list
	}
}

bool dae::InputManager::IsPressed(GamepadButton button, PlayerId id) const
{
	return m_GamepadInputManagers[id].IsPressed(static_cast<unsigned int>(button));
}

bool dae::InputManager::IsDownThisFrame(GamepadButton button, PlayerId id) const
{
	return  m_GamepadInputManagers[id].IsDownThisFrame(static_cast<unsigned int>(button));
}

bool dae::InputManager::IsUpThisFrame(GamepadButton button, PlayerId id) const
{
	return  m_GamepadInputManagers[id].IsUpThisFrame(static_cast<unsigned int>(button));
}

void dae::InputManager::Update()
{
	if (m_Data.MaxControllers > 0)
	{
		UpdateGamepadController();
		ProcessControllerInputs();
	}

	if (m_Data.AllowKeyboard)
	{
		ProcessKeyboardInputs();
	}
}

void dae::InputManager::Reset()
{
	m_GamepadEvents.clear();
	m_KeyboardEvents.clear();
	m_DisabledPlayerIds.clear();
	m_CurrentPlayerId = 0;
}

void dae::InputManager::SetData(const InputControllerData& data)
{
	m_Data = data;

	m_Data.MaxControllers = m_Data.MaxControllers > XUSER_MAX_COUNT ? XUSER_MAX_COUNT : m_Data.MaxControllers;

	if (m_Data.MaxControllers > m_GamepadInputManagers.size())
	{
		m_GamepadInputManagers.reserve(m_Data.MaxControllers);

		for (size_t i = m_GamepadInputManagers.size(); i < m_Data.MaxControllers; i++)
		{
			m_GamepadInputManagers.push_back(GamepadInputManager(i)); // Add to list
		}
	}
	else if(m_Data.MaxControllers < m_GamepadInputManagers.size())
	{
		m_GamepadInputManagers.resize(m_Data.MaxControllers);
	}

	
}

bool dae::InputManager::GetNextAvailableControllerInstance(ControllerInstance& out)
{
	if (m_CurrentPlayerId == 0 && m_Data.AllowKeyboard)
	{
		out.ControllerType = ControllerType::Both;
		out.PlayerId = m_CurrentPlayerId;
		++m_CurrentPlayerId;
		
		return true;
	}
	else if (m_CurrentPlayerId < m_Data.MaxControllers)
	{
		out.ControllerType = ControllerType::Gamepad;
		out.PlayerId = m_CurrentPlayerId;
		++m_CurrentPlayerId;

		return true;
	}

	return false;
}

void dae::InputManager::EnableInput(PlayerId playerId)
{
	m_DisabledPlayerIds.erase(playerId);
}

void dae::InputManager::DisableInput(PlayerId playerId)
{
	m_DisabledPlayerIds.insert(playerId);
}

void dae::InputManager::UpdateGamepadController()
{
	for (size_t i = 0; i < m_GamepadInputManagers.size(); i++)
	{
		m_GamepadInputManagers[i].Update(m_Data);
	}
}

void dae::InputManager::ProcessControllerInputs()
{
	int currentId = 0;

	for (size_t i = 0; i < m_GamepadEvents.size(); i++)
	{
		auto& gamepadEvent = m_GamepadEvents[i];

		if (m_DisabledPlayerIds.contains(gamepadEvent.PlayerId))
		{
			continue;
		}

		currentId = gamepadEvent.PlayerId;

		switch (gamepadEvent.ButtonState)
		{
		case ButtonState::BUTTON_DOWN:
			if (IsDownThisFrame(gamepadEvent.GamepadButton, currentId))
				gamepadEvent.OnTriggered->Execute(*gamepadEvent.PlayerGameObject);
			break;

		case ButtonState::BUTTON_PRESSED:
			if (IsPressed(gamepadEvent.GamepadButton, currentId))
				gamepadEvent.OnTriggered->Execute(*gamepadEvent.PlayerGameObject);
			break;

		case ButtonState::BUTTON_UP:
			if (IsUpThisFrame(gamepadEvent.GamepadButton, currentId))
				gamepadEvent.OnTriggered->Execute(*gamepadEvent.PlayerGameObject);
			break;
		}
	}
}

void dae::InputManager::ProcessKeyboardInputs()
{
	if (m_DisabledPlayerIds.contains(0))
	{
		return;
	}

	SDL_PumpEvents();
	const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

	for (size_t i = 0; i < m_KeyboardEvents.size(); i++)
	{
		auto& keyboardEvent = m_KeyboardEvents[i];
		bool triggered = false;

		if (!m_KeyboardKeyStates[keyboardEvent.Key] && keyboardState[keyboardEvent.Key])
		{
			m_KeyboardKeyStates[keyboardEvent.Key] = true;
			triggered = keyboardEvent.ButtonState == ButtonState::BUTTON_DOWN;
		}
		else if (m_KeyboardKeyStates[keyboardEvent.Key] && !keyboardState[keyboardEvent.Key])
		{
			m_KeyboardKeyStates[keyboardEvent.Key] = false;
			triggered = keyboardEvent.ButtonState == ButtonState::BUTTON_UP;
		}
		else if(keyboardState[keyboardEvent.Key])
		{
			triggered = keyboardEvent.ButtonState == ButtonState::BUTTON_PRESSED;
		}
		
		if (triggered)
		{
			keyboardEvent.OnTriggered->Execute(*keyboardEvent.PlayerGameObject);
		}
	}
}

dae::InputManager::GamepadInputManager::GamepadInputManager(size_t controllerIndex)
	:m_ControllerIndex(controllerIndex)
{
	ZeroMemory(&m_PreviousState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
}

void dae::InputManager::GamepadInputManager::Update(InputControllerData& data)
{
	CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));

	DWORD dwResult;
	dwResult = XInputGetState((DWORD)m_ControllerIndex, &m_CurrentState);

	bool connected{ dwResult == ERROR_SUCCESS };

	if (m_IsConnectedPreviousframe != connected)
	{
		if (connected) 
		{
			std::cout << "Controller " << m_ControllerIndex << " connected" << std::endl;
			data.OnConnected.Invoke();
		}
		else 
		{
			std::cout << "Controller " << m_ControllerIndex << " disconnected" << std::endl;
			data.OnDisconnected.Invoke();
		}
	}

	m_IsConnectedPreviousframe = connected;

	auto buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
	m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
	m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
}

bool dae::InputManager::GamepadInputManager::IsDownThisFrame(unsigned button) const
{
	return m_ButtonsPressedThisFrame & button;
}

bool dae::InputManager::GamepadInputManager::IsUpThisFrame(unsigned button) const
{
	return m_ButtonsReleasedThisFrame & button;
}

bool dae::InputManager::GamepadInputManager::IsPressed(unsigned button) const
{
	return m_CurrentState.Gamepad.wButtons & button;
}

#pragma endregion
