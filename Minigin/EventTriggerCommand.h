#pragma once

#include "Command.h"
#include "EventSystem.h"

namespace dae {

	class EventTriggerCommand final: public ICommand{

	public:
		EventTriggerCommand(const Event<GameObject&>& event) : m_Event(event){}

		virtual void Execute(GameObject& gameObject) override {

			m_Event.Invoke(gameObject);
		}
	private:
		Event<GameObject&> m_Event;

	};
}