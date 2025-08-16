#pragma once
#include <mutex>
#include <queue>
#include <unordered_map>
#include <SDL.h>
#include <SDL_mixer.h>

#include "AudioService.h"

namespace dae {

    struct MixChunkData {
        Mix_Chunk* Mix_Chunk{ nullptr };
        int LoopNumber{ 0 };
    };

    struct MusicData {
        std::string File;
        Mix_Music* Mix_Music{ nullptr };
        int LoopNumber{ 0 };
    };

    class AudioManager final : public IAudioService
    {
    public:
        AudioManager();
        virtual ~AudioManager();
        AudioManager(const AudioManager& other) = delete;
        AudioManager(AudioManager&& other) = delete;
        AudioManager& operator=(const AudioManager& other) = delete;
        AudioManager& operator=(AudioManager&& other) = delete;

        void Update() override;

        void StopSound(int channelId = -1) override;
        void Shutdown()override;
        void RequestStop()override;

        void PlaySound(SoundKey soundID) override;
        void PlayMusic() override;
        void PauseMusic() override;
        void ResumeMusic() override;

        SoundKey LoadSound(const std::string& file, int loopNumber = 0) override;
        void LoadMusic(const std::string& file, int loopNumber = -1) override;

        void SetEffectVolume(int volume) override;
        void SetMusicVolume(int volume) override;

        void ToggleMute() override;

        bool IsRunning() const override{ return m_Running.load(std::memory_order_relaxed); }

    private:
        void Initialize();


        bool m_IsMute{ false };
        std::atomic<bool> m_AudioOpen{ false };  // set true after Mix_OpenAudio succeeds
        std::atomic<bool> m_Running{ true };       // was bool
        MusicData m_Music;
        std::mutex m_Mutex;
        std::condition_variable m_ConditionVariable;
        std::queue<int> m_Queue;
        std::unordered_map<std::string, SoundKey> m_FileName_To_SoundKey;
        std::vector<MixChunkData> m_Sounds;

    };
}

