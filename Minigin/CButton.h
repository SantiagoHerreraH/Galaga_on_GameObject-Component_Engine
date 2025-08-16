#pragma once

#include <string>
#include <vector>
#include <glm.hpp>
#include "SDL.h"
#include "EventSystem.h"
#include "Scene.h"
#include "InputManager.h"
#include "CText.h"

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
		bool IsSelected()const { return m_IsSelected; }
		void Select();
		void Unselect();
		void Press();
		void SubscribeOnPress(const std::function<void()>& func);
	private:

		bool m_IsSelected{ false };
		std::shared_ptr<CText> m_CText;
		ButtonData m_ButtonData;
	};

	
}