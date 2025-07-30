#include "Lifetime.h"

dae::CLifeTime::CLifeTime(float lifetimeInSeconds, bool startDeactivated) :
	m_Lifetime(lifetimeInSeconds),
	m_StartDeactivated(startDeactivated)
{
}

void dae::CLifeTime::Start()
{
	m_CurrentTimerSystem = &TimerSystem::GetCurrent();

	CLifeTime* self{ this };

	auto deactiveAction = [self]() mutable 
		{
			self->Owner().SetActive(false);
			self->m_OnEndLifeTime.Invoke();
		};
	Event<> onTimerEndEvent{};
	onTimerEndEvent.Subscribe(deactiveAction);

	Timer lifetime{ m_Lifetime, m_StartDeactivated, "LifeTime of " + Owner().GetName()};
	lifetime.SetOnEndEvent(onTimerEndEvent);
	m_CurrentLifetimeTimerKey = m_CurrentTimerSystem->AddTimer(lifetime);

	if (m_StartDeactivated)
	{
		Owner().SetActive(false);
	}
}

void dae::CLifeTime::SubscribeOnEndLifetime(const std::function<void()>& func)
{
	m_OnEndLifeTime.Subscribe(func);
}

void dae::CLifeTime::Respawn(const glm::vec2& where)
{
	Owner().SetActive(true);
	Owner().Transform().SetLocalPositionX(where.x);
	Owner().Transform().SetLocalPositionY(where.y);

	m_CurrentTimerSystem->RestartTimer(m_CurrentLifetimeTimerKey);
}
