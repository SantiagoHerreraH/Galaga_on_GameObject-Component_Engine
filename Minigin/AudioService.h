#pragma once

#include <string>

namespace dae {


    using SoundKey = int;
	class IAudioService {

	public:

        IAudioService() = default;
        virtual ~IAudioService() = default;
        IAudioService(const IAudioService& other) = delete;
        IAudioService(IAudioService&& other) = delete;
        IAudioService& operator=(const IAudioService& other) = delete;
        IAudioService& operator=(IAudioService&& other) = delete;

		virtual void Update() = 0;

        virtual void StopSound(int channelId = -1) = 0;
        virtual void StopAllSounds() = 0;

        virtual void PlaySound(SoundKey soundID) = 0;
        virtual void PlayMusic() = 0;
        virtual void PauseMusic() = 0;
        virtual void ResumeMusic() = 0;

        virtual SoundKey LoadSound(const std::string& file, int loopNumber = 0) = 0;
        virtual void LoadMusic(const std::string& file, int loopNumber = -1) = 0;

        virtual void SetEffectVolume(int volume) = 0;
        virtual void SetMusicVolume(int volume) = 0;

        virtual void ToggleMute() = 0;
	};

    class NullAudioService  : IAudioService{
    public:

        virtual void Update() {}

        virtual void StopSound(int) {}
        virtual void StopAllSounds() {}

        virtual void PlaySound(SoundKey ) {}
        virtual void PlayMusic() {}
        virtual void PauseMusic() {}
        virtual void ResumeMusic() {}

        virtual SoundKey LoadSound(const std::string& , int ) {}
        virtual void LoadMusic(const std::string& , int ) {}

        virtual void SetEffectVolume(int ) {}
        virtual void SetMusicVolume(int ) {}


        virtual void ToggleMute(){}
    };

}