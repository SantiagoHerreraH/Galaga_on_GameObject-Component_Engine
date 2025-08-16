#include "AudioManager.h"
#include "ResourceManager.h"
#include <iostream>

namespace dae {

	AudioManager::AudioManager()
		: m_Running( true )
	{
		Initialize();
	}

	AudioManager::~AudioManager()
	{
		for (size_t i = 0; i < m_Sounds.size(); i++)
		{
			Mix_FreeChunk(m_Sounds[i].Mix_Chunk);
		}

		Mix_FreeMusic(m_Music.Mix_Music);
	}

	void AudioManager::Initialize()
	{

		int flags = MIX_INIT_MP3;
		if ((Mix_Init(flags) & flags) != flags) {
			std::cerr << "Mix_Init failed: " << Mix_GetError() << "\n";
		}

		if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
			std::cerr << "SDL_InitSubSystem(SDL_INIT_AUDIO) failed: " << SDL_GetError() << "\n";
			return;
		}

		if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
			std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << "\n";
			return;
		}
		Mix_AllocateChannels(16);
		m_AudioOpen.store(true, std::memory_order_relaxed);

	}

	void AudioManager::Update()
	{
		// Wait briefly for new work, then drain the queue and return.
		// This makes Update() cooperative and responsive to shutdown.
		std::unique_lock<std::mutex> lock(m_Mutex);
		// Wake either when there's a notify, or after 10ms to re-check running state.
		m_ConditionVariable.wait_for(lock, std::chrono::milliseconds(10), [&] {
			return !m_Queue.empty() || !m_Running.load(std::memory_order_relaxed);
			});

		while (!m_Queue.empty() && m_Running.load(std::memory_order_relaxed)) {
			int id = m_Queue.front();
			m_Queue.pop();
			auto& s = m_Sounds.at(static_cast<size_t>(id));
			Mix_PlayChannel(-1, s.Mix_Chunk, s.LoopNumber);
		}
	}

	void AudioManager::RequestStop() {
		m_Running.store(false, std::memory_order_relaxed);
		m_ConditionVariable.notify_all();   // wake Update() if waiting
	}

	void AudioManager::PlaySound(SoundKey soundID)
	{
		//Mix_PlayChannel(-1, m_pSounds.at(soundID), 0);
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Queue.push(soundID);
		m_ConditionVariable.notify_one();
	}

	void AudioManager::StopSound(int channelId)
	{
		Mix_HaltChannel(channelId);
	}

	void AudioManager::Shutdown()
	{

		if (!m_AudioOpen.exchange(false, std::memory_order_acq_rel))
			return; // already closed

		// Stop playback safely
		Mix_HaltChannel(-1);
		if (Mix_PlayingMusic()) 
		{
			Mix_HaltMusic();
		}
		Mix_CloseAudio();
	}

	void AudioManager::PlayMusic()
	{
		Mix_PlayMusic(m_Music.Mix_Music, -1);
	}

	void AudioManager::PauseMusic()
	{
		Mix_PauseMusic();
	}

	void AudioManager::ResumeMusic()
	{
		Mix_ResumeMusic();
	}

	SoundKey AudioManager::LoadSound(const std::string& file, int loopNumber)
	{
		if (m_FileName_To_SoundKey.contains(file))
		{
			return m_FileName_To_SoundKey[file];
		}

		const auto fullPath = ResourceManager::GetInstance().GetDataPath() / file;

		auto sound{ Mix_LoadWAV(fullPath.string().c_str())};

		if (sound == nullptr)
		{
			std::string errorMsg = "SoundStream: Failed to load " + fullPath.string() + " Sound,\nSDL_mixer Error: " + Mix_GetError();
			std::cerr << errorMsg;
		}

		m_Sounds.push_back({ sound , loopNumber});

		SoundKey value = SoundKey(m_Sounds.size()) - 1;
		m_FileName_To_SoundKey[file] = value;

		return value;
	}

	void AudioManager::LoadMusic(const std::string& file, int loopNumber)
	{
		if (!m_Music.File.empty() && m_Music.File == file)
		{
			return;
		}

		if (m_Music.Mix_Music)
		{
			Mix_FreeMusic(m_Music.Mix_Music);
		}

		const auto fullPath = ResourceManager::GetInstance().GetDataPath() / file;

		m_Music.File = file;
		m_Music.LoopNumber = loopNumber;
		m_Music.Mix_Music = Mix_LoadMUS(fullPath.string().c_str());
		if (m_Music.Mix_Music == nullptr)
		{
			std::string errorMsg = "SoundStream: Failed to load " + fullPath.string() + " Music,\nSDL_mixer Error: " + Mix_GetError();
			std::cerr << errorMsg;
		}
	}

	void AudioManager::SetEffectVolume(int volume)
	{
		Mix_Volume(-1, volume);
	}

	void AudioManager::SetMusicVolume(int volume)
	{
		Mix_VolumeMusic(volume);
	}

	void AudioManager::ToggleMute()
	{
		if (m_IsMute)
		{
			m_IsMute = false;
			SetEffectVolume(MIX_MAX_VOLUME);
			SetMusicVolume(MIX_MAX_VOLUME);
		}
		else
		{

			m_IsMute = true;
			SetEffectVolume(0);
			SetMusicVolume(0);
		}
	}

}
