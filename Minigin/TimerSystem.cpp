#include "TimerSystem.h"
#include "GameTime.h"
#include "MovementActionSequence.h"

dae::Timer::Timer()
{
}

dae::Timer::Timer(float durationInSeconds, bool startPaused, const std::string& name) :
	m_Duration(durationInSeconds),
	m_CurrentTime(0.f),
	m_IsPaused(startPaused),
	m_Name(name)
{
	SetDuration(m_Duration);
}


//this way, even if duration is 0, start update and end will be called
void dae::Timer::Update()
{
	if (!m_IsPaused)
	{
		if (m_CurrentTime == 0)
		{
			m_OnStart.Invoke();
		}

		m_CurrentTime += Time::GetInstance().GetElapsedSeconds();

		m_OnUpdate.Invoke();

		if (IsFinished())
		{
			m_OnEnd.Invoke();

		}

	}
}

float dae::Timer::GetTime() const
{
	return m_CurrentTime;
}

void dae::Timer::SetTime(float time)
{
	m_CurrentTime = time < 0.f ? 0.f : time > m_Duration ? m_Duration : time;
}

float dae::Timer::GetDuration() const
{
	return m_Duration;
}


void dae::Timer::SetDuration(float durationInSeconds)
{
	m_Duration = durationInSeconds;

	SetInfiniteDuration(durationInSeconds < 0);
}

void dae::Timer::SetInfiniteDuration(bool isDurationInfinite)
{
	m_IsDurationInfinite = isDurationInfinite;
	if (m_IsDurationInfinite)
	{
		m_Duration = INFINITY;
	}
}

bool dae::Timer::IsPaused() const
{
	return m_IsPaused;
}

void dae::Timer::ClearEvents()
{
	m_OnRestart.UnsubscribeAll();
	m_OnStart.UnsubscribeAll();
	m_OnUpdate.UnsubscribeAll();
	m_OnEnd.UnsubscribeAll();
}

void dae::Timer::End()
{
	if (!IsFinished())
	{

		m_CurrentTime = 0;
		Pause();

		m_OnEnd.Invoke();
	}
}

void dae::Timer::Pause()
{
	m_IsPaused = true;
}

void dae::Timer::Play()
{
	m_IsPaused = false;
}

void dae::Timer::Restart()
{
	m_IsPaused = false;
	m_CurrentTime = 0;
	m_OnRestart.Invoke();
}

bool dae::Timer::IsFinished()const
{
	return m_CurrentTime >= m_Duration && !m_IsDurationInfinite;
}

void dae::Timer::SetOnStartEvent(const Event<>& onStart)
{
	m_OnStart = onStart;
}

void dae::Timer::SetOnRestartEvent(const Event<>& onRestart)
{
	m_OnRestart = onRestart;
}

void dae::Timer::SetOnUpdateEvent(const Event<>& onUpdate)
{
	m_OnUpdate = onUpdate;
}

void dae::Timer::SetOnEndEvent(const Event<>& onEnd)
{
	m_OnEnd = onEnd;
}

#pragma endregion

#pragma region Time

dae::TimerKey dae::TimerSystem::AddTimer(const Timer& timer)
{
	++m_LastKey;
	m_Timers.Add(m_LastKey, timer, !timer.IsPaused());
	return m_LastKey;
}

bool dae::TimerSystem::RemoveTimer(TimerKey key)
{
	return m_Timers.Erase(key);
}

void dae::TimerSystem::ClearTimerEvents(TimerKey key)
{
	m_Timers.ValueAt(key).ClearEvents();
}

dae::Timer& dae::TimerSystem::TimerAt(TimerKey key)
{
	return m_Timers.ValueAt(key);
}

bool dae::TimerSystem::HasTimer(TimerKey key) const
{
	return m_Timers.Contains(key);
}

bool dae::TimerSystem::IsTimerActive(TimerKey key) const
{
	return m_Timers.IsActivated(key);
}

void dae::TimerSystem::RestartTimer(TimerKey key)
{
	m_Timers.Activate(key);
	m_Timers.ValueAt(key).Restart();
}

void dae::TimerSystem::EndTimer(TimerKey key)
{
	m_Timers.Deactivate(key);
	m_Timers.ValueAt(key).End();
}

void dae::TimerSystem::PauseTimer(TimerKey key)
{
	m_Timers.Deactivate(key);
	m_Timers.ValueAt(key).Pause();
}

void dae::TimerSystem::PlayTimer(TimerKey key)
{
	m_Timers.Activate(key);
	m_Timers.ValueAt(key).Play();
}

void dae::TimerSystem::SetTimerTime(TimerKey key, float time)
{
	Timer& timer = m_Timers.ValueAt(key);
	timer.SetTime(time);
	if (timer.IsFinished())
	{
		m_Timers.Deactivate(key);
	}
	else
	{
		m_Timers.Activate(key);
	}
}

void dae::TimerSystem::SetTimerDuration(TimerKey key, float duration, bool changeStateBasedOnDuration)
{
	Timer& timer = m_Timers.ValueAt(key);
	timer.SetDuration(duration);

	if (changeStateBasedOnDuration)
	{
		if (timer.IsFinished())
		{
			m_Timers.Deactivate(key);
		}
		else
		{
			m_Timers.Activate(key);
		}
	}

}

dae::TimerKey dae::TimerSystem::TriggerFunctionAfterSeconds(const std::function<void()>& function, float seconds, bool removeTimerAfterTriggering, bool startTimerPaused)
{
	Timer timer{ seconds, startTimerPaused };
	timer.GetOnEndEvent().Subscribe(function);
	
	Scene* scenePtr = GetScenePtr();

	TimerKey key = AddTimer(timer);
	if (removeTimerAfterTriggering)
	{
		TimerAt(key).GetOnEndEvent().Subscribe([key, scenePtr]()
			{

				SceneSingleton::GetFromScene(scenePtr).RemoveTimer(key);

			});
	}

	return key;
}

dae::TimerKey dae::TimerSystem::StartEndFunctionWithDuration(const std::function<void()>& start, const std::function<void()>& end, float duration, bool removeTimerAfterTriggering, bool startTimerPaused)
{
	Timer timer{ duration, startTimerPaused };
	timer.GetOnStartEvent().Subscribe(start);
	timer.GetOnEndEvent().Subscribe(end);

	Scene* scenePtr = GetScenePtr();

	TimerKey key = AddTimer(timer);
	if (removeTimerAfterTriggering)
	{
		TimerAt(key).GetOnEndEvent().Subscribe([key, scenePtr]()
			{

				SceneSingleton::GetFromScene(scenePtr).RemoveTimer(key);

			});
	}

	return key;
}

void dae::TimerSystem::Update()
{
	for (size_t i = 0; i < m_Timers.ActivatedSize(); )
	{
		m_Timers.At(i).Update();
		if (m_Timers.At(i).IsFinished())
		{
			m_Timers.DeactivateAt(i);
		}
		else
		{
			++i;
		}
	}
}

void dae::TimerSystem::Reset()
{
	m_Timers.Clear();
	m_LastKey = 0;
}

size_t dae::TimerSystem::GetActivatedTimersSize() const
{
	return m_Timers.ActivatedSize();
}