#pragma once

#include <string>
#include "Component.h"
#include "AudioManager.h"

namespace dae {

	struct AudioData{

		std::string File;
		int LoopAmount = 0; //negative == infinite
	};

	class CAudio final : public Component
	{
	public:

		CAudio(const AudioData& audioData);
		void Start()override;
		void Play();

	private:
		bool m_PlaySoundOnStart{ false };
		SoundKey m_SoundKey{-1};
		AudioData m_AudioData{};
	};

}


