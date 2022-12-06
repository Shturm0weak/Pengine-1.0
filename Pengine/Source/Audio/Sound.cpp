#include "Sound.h"

#include "wavParser.h"
#include "vorbis.h"

#include "../Core/Logger.h"
#include "../Core/Utils.h"

using namespace Pengine;

void Sound::SetVolume(float volume) const
{
	alSourcef(m_Source, AL_GAIN, volume);
}

Sound::Sound(const std::string& filePath)
{
	SetFilePath(filePath);
	SetName(filePath);

	std::string format = Utils::GetResolutionFromFilePath(filePath);
	if (format == "ogg")
	{
		m_NumberOfSamples = stb_vorbis_decode_filename(m_FilePath.c_str(), &m_Channels, &m_SampleRate, &m_SoundBuffer);
		if (m_NumberOfSamples < 0)
		{
			Logger::Error("Failed to load!", "Sound", m_FilePath.c_str());
		}
		else
		{
			Logger::Success("has been loaded!", "Sound", m_FilePath.c_str());
		}
	}
	else if (format == "wav")
	{
		m_NumberOfSamples = WavReader(m_FilePath.c_str(), &m_Channels, &m_SampleRate, &m_SoundBuffer);
		if (m_NumberOfSamples < 0)
		{
			Logger::Error("Failed to load!", "Sound", m_FilePath.c_str());
		}
		else
		{
			Logger::Success("has been loaded!", "Sound", m_FilePath.c_str());
		}
	}
}

Sound::~Sound()
{
	if (m_SoundBuffer)
	{
		delete m_SoundBuffer;
	}
}