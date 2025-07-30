#pragma once

#include <string>
#include "SceneSingleton.h"
#include "EventSystem.h"
#include "ActivatableFlatMap.h"

namespace dae {

	class TimerSystem;

	class Timer {

	public:
		Timer();
		Timer(float durationInSeconds, bool startPaused, const std::string& name = "NoName");

		void Update();

		float GetTime()const;
		float GetDuration()const;

		bool IsPaused()const;
		bool IsFinished()const;

		void SetOnStartEvent(const Event<>& onStart);
		void SetOnRestartEvent(const Event<>& onRestart);
		void SetOnUpdateEvent(const Event<>& onUpdate);
		void SetOnEndEvent(const Event<>& onEnd);

		Event<>& GetOnStartEvent() { return m_OnStart; }
		Event<>& GetOnRestartEvent() { return m_OnRestart; }
		Event<>& GetOnUpdateEvent() { return m_OnUpdate; }
		Event<>& GetOnEndEvent() { return m_OnEnd; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:

		friend class TimerSystem;

		void ClearEvents();
		void End();
		void Pause();
		void Play();
		void Restart();
		void SetTime(float time);
		void SetDuration(float durationInSeconds);//if negative == infinite duration
		void SetInfiniteDuration(bool isDurationInfinite);

		float m_Duration{ 0.0f };
		float m_CurrentTime{ 0.0f };
		bool m_IsDurationInfinite{ false };

		bool m_IsPaused = false;

		std::string m_Name;

		Event<> m_OnRestart{};
		Event<> m_OnStart{};
		Event<> m_OnUpdate{};
		Event<> m_OnEnd{};

	};

	using TimerKey = size_t;

	class TimerSystem final : public SceneSingleton<TimerSystem>
	{
	public:
		void Update()override;
		void Reset()override;

		size_t GetActivatedTimersSize()const;
		TimerKey AddTimer(const Timer& timer);
		bool RemoveTimer(TimerKey key);
		void ClearTimerEvents(TimerKey key);
		Timer& TimerAt(TimerKey key);
		bool HasTimer(TimerKey key)const;
		bool IsTimerActive(TimerKey key)const;
		void RestartTimer(TimerKey key);
		void EndTimer(TimerKey key);
		void PauseTimer(TimerKey key);
		void PlayTimer(TimerKey key);
		void SetTimerTime(TimerKey key, float time);
		void SetTimerDuration(TimerKey key, float duration, bool changeStateBasedOnDuration = true);
		TimerKey TriggerFunctionAfterSeconds(const std::function<void()>& function, float seconds, bool removeTimerAfterTriggering, bool startTimerPaused);
		TimerKey StartEndFunctionWithDuration(const std::function<void()>& start, const std::function<void()>& end, float duration, bool removeTimerAfterTriggering, bool startTimerPaused);

		//General rule for functions here is that first you deactivate/activate timers -> 
		//then you call the end(), pause(), play() functions which call events, because those events can trigger activation code once again

	private:

		ActivatableFlatMap<TimerKey, Timer> m_Timers;
		size_t m_LastKey{};
	};

}

