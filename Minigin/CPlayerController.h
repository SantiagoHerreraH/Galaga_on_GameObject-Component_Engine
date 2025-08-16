#pragma once

#include "Component.h"
#include "InputManager.h"
#include <SDL.h>


namespace dae {

	struct PlayerGamepadKeyData {
		ButtonState ButtonState;
		GamepadButton GamepadButton;
		std::shared_ptr<ICommand> OnTriggered;
	};
	struct PlayerKeyboardKeyData {
		ButtonState ButtonState;
		SDL_Scancode Key;
		std::shared_ptr<ICommand> OnTriggered;
	};

	class CPlayerController final : public Component
	{
	public:
		CPlayerController();
		
		void Start()override;
		void SetActive(bool isActive)override;

		bool IsValid()const;
		ControllerType GetControllerType();
		bool BindKey(PlayerGamepadKeyData playerGamepadKeyData);
		bool BindKey(PlayerKeyboardKeyData playerKeyboardKeyData);

	private:

		bool m_IsValid{false};
		ControllerInstance m_ControllerInstance;
		std::vector<PlayerGamepadKeyData> m_PlayerGamepadKeyData;
		std::vector<PlayerKeyboardKeyData> m_PlayerKeyboardKeyData;
	};
}


