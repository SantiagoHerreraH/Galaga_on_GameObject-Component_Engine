#pragma once

#include <glm.hpp>
#include "Component.h"
#include "TimerSystem.h"

namespace dae {


	class CLifeTime : public Component {

	public:
		CLifeTime(float lifetimeInSeconds, bool startDeactivated);
		void Start()override;
		void SubscribeOnEndLifetime(const std::function<void()>& func);
		void Respawn(const glm::vec2& where);

	private:
		Event<> m_OnEndLifeTime;
		TimerSystem* m_CurrentTimerSystem;
		TimerKey m_CurrentLifetimeTimerKey;
		float m_Lifetime;
		bool m_StartDeactivated;
	};
}