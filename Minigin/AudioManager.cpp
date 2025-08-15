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
		int result = Mix_Init(flags);
		if ((result & flags) != flags) {
			printf("Mix_Init: Failed to init required ogg and mod support!\n");
			printf("Mix_Init: %s\n", Mix_GetError());
		}

		// start SDL with audio support
		if (SDL_Init(SDL_INIT_AUDIO) == -1) {
			printf("SDL_Init: %s\n", SDL_GetError());
			exit(1);
		}
		// open 44.1KHz, signed 16bit, system byte order,
		//      stereo audio, using 1024 byte chunks
		if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
			printf("Mix_OpenAudio: %s\n", Mix_GetError());
			exit(2);
		}
		// 8 mixing channels get created
		MIX_CHANNELS;
	}

	void AudioManager::Update()
	{
		while(m_Running)
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_ConditionVariable.wait(lock);

			while (!m_Queue.empty() && m_Running)
			{
				int id = m_Queue.front();
				m_Queue.pop();
				Mix_PlayChannel(-1, m_Sounds.at(id).Mix_Chunk, m_Sounds.at(id).LoopNumber);
			}
		}
		std::cout << "Exiting console audio update" << std::endl;
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

	void AudioManager::StopAllSounds()
	{
		//Dont use mix functions after this, else call openAudio first
		Mix_CloseAudio();
	
		m_Running = false;
		m_ConditionVariable.notify_one();
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
