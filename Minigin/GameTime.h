#pragma once

#include "Singleton.h"

namespace dae {

	class Time : public Singleton<Time>
	{
		friend class Minigin;

	public:
		int GetFramesPerSecond() { return int(1.f / Time::GetInstance().GetElapsedSeconds()); }
		float GetElapsedSeconds() { return m_ElapsedSeconds; };
		float GetFixedTimeStep() { return m_FixedTimeStep; }

		
	private:
		float m_ElapsedSeconds{0.f};
		float m_FixedTimeStep{ 0.f };

	};
}

