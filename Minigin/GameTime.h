#pragma once

#include "Singleton.h"

namespace dae {

	class GameTime : public Singleton<GameTime>
	{
		friend class Minigin;

	public:
		int GetFramesPerSecond() { return int(1.f / GameTime::GetInstance().GetElapsedSeconds()); }
		float GetElapsedSeconds() { return m_ElapsedSeconds; };
		float GetFixedTimeStep() { return m_FixedTimeStep; }

		
	private:
		float m_ElapsedSeconds{0.f};
		float m_FixedTimeStep{ 0.f };

	};
}

