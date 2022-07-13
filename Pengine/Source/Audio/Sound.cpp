#include "Sound.h"
#include "wavParser.h"
#include "vorbis.h"

#include "../Core/Logger.h"
//#include "../Core/Utils.h"

using namespace Pengine;

void Sound::SetVolume(float volume) const
{
	alSourcef(m_Source, AL_GAIN, volume);
}

Sound::Sound(const std::string& filePath)
{
	SetFilePath(filePath);
	SetName(filePath);//Utils::GetNameFromFilePath(filePath));

	std::string format = m_FilePath.substr(m_FilePath.find_last_of(".") + 1);
	if (format == "ogg")
	{
		m_NumberOfSamples = stb_vorbis_decode_filename(m_FilePath.c_str(), &m_Channels, &m_SampleRate, &m_SoundBuffer);
		if (m_NumberOfSamples < 0)
		{
			Logger::Error("Failed to load", m_FilePath.c_str(), "Sound");
			return;
		}
	}
	else if (format == "wav")
	{
		m_NumberOfSamples = WavReader(m_FilePath.c_str(), &m_Channels, &m_SampleRate, &m_SoundBuffer);
		if (m_NumberOfSamples < 0)
		{
			Logger::Error("Failed to load", m_FilePath.c_str(), "Sound");
			return;
		}
	}
}

Sound::~Sound()
{
	delete m_SoundBuffer;
}