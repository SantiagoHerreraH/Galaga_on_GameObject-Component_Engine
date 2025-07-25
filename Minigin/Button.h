#pragma once

#include <string>
#include <vector>
#include <glm.hpp>
#include "SDL.h"
#include "EventSystem.h"
#include "Scene.h"
#include "InputManager.h"
#include "Text.h"

namespace dae {

	struct ButtonData {
		std::string Name;
		FontData FontData;
		SDL_Color UnselectedColor{ 255, 255 , 255 };
		SDL_Color SelectedColor{ 255, 0 , 0 };
		dae::Event<> OnPress;
	};

	class CButton : public Component{
		
	public:
		CButton(const ButtonData& buttonData);
		void Start()override;
		void Select();
		void Unselect();
		void Press();
		void SubscribeOnPress(const std::function<void()>& func);
	private:

		std::shared_ptr<CText> m_CText;
		ButtonData m_ButtonData;
	};

	
}