#pragma once

#include "../Core/Core.h"
#include "../Core/Asset.h"
#include "../Enums/ColoredOutput.h"

#include <string>
#include <iostream>
#include <map>

#include "OpenAl/al.h"
#include "OpenAl/alc.h"

namespace Pengine
{

	enum AudioState
	{
		PLAYING,
		STOPPED,
		PAUSED
	};

	class PENGINE_API Sound : public IAsset
	{
	private:

		ALuint m_Buffer;
		ALuint m_Source;
		AudioState m_State;
		short* m_SoundBuffer;
		float m_Volume;
		int m_Channels;
		int m_SampleRate;
		int m_NumberOfSamples;
		bool m_IsLooped;

		Sound(const std::string& filePath);
		~Sound();
		
		friend class SoundManager;
	public:
		
		void SetVolume(float volume) const;

		int GetState() const { return (int)m_State; }

		ALuint GetId() const { return m_Source; }

		virtual void Reload() override {}
	};

}