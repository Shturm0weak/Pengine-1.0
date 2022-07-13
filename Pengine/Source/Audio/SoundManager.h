#pragma once

#include "Sound.h"

namespace Pengine
{

	class PENGINE_API SoundManager
	{
	private:

		std::map<std::string, Sound*> m_Sounds;

		ALCdevice* m_AlcDevice = nullptr;
		ALCcontext* m_AlcContext = nullptr;
		
		float m_CurrentVolume = 1.0f;

		void CreateSoundAsset(Sound* sound);

		SoundManager();
		SoundManager(const SoundManager&) = delete;
		SoundManager& operator=(const SoundManager&) { return *this; }
	public:

		float m_MasterVolume = 1.0f;

		static SoundManager& GetInstance();

		Sound* GetSound(const std::string& name) const;

		Sound* Load(const std::string& filePath);

		void Resume(Sound* sound) const;

		void Play(Sound* sound) const;
		
		void Loop(Sound* sound) const;
		
		void Pause(Sound* sound) const;
		
		void Stop(Sound* sound) const;
		
		void SetVolume(float volume);
		
		void UpdateVolume();
		
		void UpdateSourceState() const;
		
		void ShutDown();
	};

}