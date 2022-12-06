#include "SoundManager.h"

#include "../Core/Logger.h"
#include "../Core/Utils.h"

using namespace Pengine;

SoundManager::SoundManager()
{
	m_AlcDevice = alcOpenDevice(NULL);
	m_AlcContext = alcCreateContext(m_AlcDevice, NULL);
	alcMakeContextCurrent(m_AlcContext);
	Logger::Success("Sound manager has been initialized!");
}

void SoundManager::CreateSoundAsset(Sound* sound)
{
	alGenBuffers(1, &sound->m_Buffer);
	alBufferData(sound->m_Buffer, AL_FORMAT_STEREO16, sound->m_SoundBuffer, sound->m_NumberOfSamples * sound->m_Channels * 2, sound->m_SampleRate);
	alGenSources(1, &sound->m_Source);
	alSourcei(sound->m_Source, AL_BUFFER, sound->m_Buffer);
	m_Sounds.insert(std::make_pair(sound->GetName(), sound));

	sound->m_Volume = 1.0f;
	sound->m_State = STOPPED;
}

void SoundManager::Resume(Sound* sound) const
{
	alSourcePlay(sound->GetId());
}

void SoundManager::Play(Sound* sound) const
{
	alSourceStop(sound->GetId());
	Resume(sound);
}

void SoundManager::Loop(Sound* sound) const
{
	alSourceStop(sound->GetId());
	alSourcei(sound->GetId(), AL_LOOPING, AL_TRUE);
	alSourcePlay(sound->GetId());
}

void SoundManager::Pause(Sound* sound) const
{
	alSourcePause(sound->GetId());
}

void SoundManager::Stop(Sound* sound) const
{
	alSourceStop(sound->GetId());
}

SoundManager& SoundManager::GetInstance()
{
	static SoundManager instance;
	return instance;
}

Sound* SoundManager::GetSound(const std::string& name) const
{
	auto sound = m_Sounds.find(name);
	if (sound != m_Sounds.end())
	{
		return sound->second;
	}
	return nullptr;
}

Sound* SoundManager::Load(const std::string& filePath)
{
	Sound* sound = GetSound(filePath);
	if (!sound)
	{
		sound = new Sound(filePath);
		if (sound->m_NumberOfSamples < 0)
		{
			delete sound;
		}
		else
		{
			CreateSoundAsset(sound);
		}
	}

	return sound;
}

void SoundManager::SetVolume(float volume)
{
	m_CurrentVolume = volume;
	alListenerf(AL_GAIN, m_CurrentVolume * m_MasterVolume);
}

void SoundManager::UpdateVolume()
{
	SetVolume(m_CurrentVolume);
}

void SoundManager::UpdateSourceState() const
{
	for (auto i = m_Sounds.begin(); i != m_Sounds.end(); i++)
	{
		ALint state;
		alGetSourcei(i->second->GetId(), AL_SOURCE_STATE, &state);

		switch (state)
		{
		case AL_PAUSED:
			i->second->m_State = PAUSED;
			break;
		case AL_PLAYING:
			i->second->m_State = PLAYING;
			break;
		case AL_INITIAL:
		case AL_STOPPED:
			i->second->m_State = STOPPED;
			break;
		default:
			i->second->m_State = STOPPED;
			break;
		}
	}
}

void SoundManager::ShutDown()
{
	for (auto i = m_Sounds.begin(); i != m_Sounds.end(); i++)
	{
		delete i->second;
	}
	m_Sounds.clear();
}
