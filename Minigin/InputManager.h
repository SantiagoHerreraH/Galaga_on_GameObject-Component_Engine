#pragma once
#include "Singleton.h"
#include <tuple>
#include <map>
#include <memory>

#include "Command.h"
#include "EventSystem.h"

#include <SDL.h>
#include <Windows.h>
#include <Xinput.h>

namespace dae
{
	using PlayerId = int;

	enum class GamepadButton
	{
		ButtonA = 0x1000,
		ButtonB = 0x2000,
		ButtonX = 0x4000,
		ButtonY = 0x8000,
		DpadUp = 0x0001,
		DpadDown = 0x0002,
		DpadLeft = 0x0004,
		DpadRight = 0x0008,
		LeftThumb = 0x0040,
		RightThumb = 0x0080,
		LeftShoulder = 0x0100,
		RightShoulder = 0x0200,
		Start = 0x0010,
		Back = 0x0020
	};

	enum class ButtonState
	{
		BUTTON_DOWN,
		BUTTON_PRESSED,
		BUTTON_UP
	};

	enum class ControllerType {
		Both,
		Gamepad,
		Keyboard,
	};

	struct ControllerInstance {
		ControllerType ControllerType{ControllerType::Both};
		PlayerId PlayerId{0};
	};

	
	struct GamepadKeyData {
		ButtonState ButtonState;
		GamepadButton GamepadButton;
		PlayerId PlayerId;
		GameObject* PlayerGameObject;
		std::shared_ptr<ICommand> OnTriggered;
	}; 
	struct KeyboardKeyData {
		ButtonState ButtonState;
		SDL_Scancode Key;
		GameObject* PlayerGameObject;
		std::shared_ptr<ICommand> OnTriggered;
	};
	using GamepadEvents = std::vector<GamepadKeyData>;
	using KeyboardEvents = std::vector<KeyboardKeyData>;

	enum class HowToTreatKeyboardController {
		KeyboardIsAnotherController,
		KeyboardIsTheSameAsFirstGamepadController,
	};

	struct InputControllerData {
		bool AllowKeyboard = true;
		HowToTreatKeyboardController HowToTreatKeyboardController;
		int MaxControllers = XUSER_MAX_COUNT;
		Event<> OnConnected{};
		Event<> OnDisconnected{};
	};

	class InputManager final : public Singleton<InputManager>
	{
	public:
		InputManager(const InputControllerData& defaultData = InputControllerData{});
		virtual ~InputManager() noexcept{}
		InputManager(const InputManager& other) = delete;
		InputManager(InputManager&& other) = delete;
		InputManager& operator=(const InputManager& other) = delete;
		InputManager& operator=(InputManager&& other) = delete;

		void SetData(const InputControllerData& data);
		bool GetNextAvailableControllerInstance(ControllerInstance& out); //return false if no available controller instance
		void ProcessInput();


		void ClearKeys();
		void BindKey(const GamepadKeyData& gamepadKeyData);
		void BindKey(const KeyboardKeyData& keyboardKeyData);

	private:

		GamepadEvents m_GamepadEvents{};
		KeyboardEvents m_KeyboardEvents{};
		bool m_Cleared{};

		bool IsPressed(GamepadButton button, PlayerId id) const;
		bool IsDownThisFrame(GamepadButton button, PlayerId id) const;
		bool IsUpThisFrame(GamepadButton button, PlayerId id) const;

		void UpdateGamepadController();
		void ProcessControllerInputs();
		void ProcessKeyboardInputs();

		class GamepadInputManager
		{

		public:
			GamepadInputManager(size_t controllerIndex = 0);

			void Update(InputControllerData& data);

			bool IsDownThisFrame(unsigned button) const;

			bool IsUpThisFrame(unsigned button) const;

			bool IsPressed(unsigned button) const;

		private:

			bool m_IsConnectedPreviousframe{ false };

			XINPUT_STATE m_PreviousState{};
			XINPUT_STATE m_CurrentState{};

			WORD m_ButtonsPressedThisFrame{};
			WORD m_ButtonsReleasedThisFrame{};

			size_t m_ControllerIndex;
		};

		std::vector<bool> m_KeyboardKeyStates;
		std::vector<GamepadInputManager> m_GamepadInputManagers;
		InputControllerData m_Data;
		PlayerId m_CurrentPlayerId;
	};

	
}