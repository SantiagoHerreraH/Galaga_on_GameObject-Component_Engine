#include "Audio.h"
#include "ServiceLocator.h"
#include "GameObject.h"

dae::CAudio::CAudio(const AudioData& audioData) : m_AudioData(audioData){}

void dae::CAudio::Start()
{
	m_SoundKey = ServiceLocator::GetInstance().GetService<IAudioService>()->LoadSound(m_AudioData.File, m_AudioData.LoopAmount);
	
	if (m_PlaySoundOnStart)
	{
		m_PlaySoundOnStart = false;
		Play();
	}
}

void dae::CAudio::Play() 
{
	if (HasOwner() && m_SoundKey >= 0)
	{
		ServiceLocator::GetInstance().GetService<IAudioService>()->PlaySound(m_SoundKey);
	}
	else
	{
		m_PlaySoundOnStart = true;
	}

}