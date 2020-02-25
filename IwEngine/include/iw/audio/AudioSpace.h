#pragma once

#include "IwAudio.h"
#include "fmod/fmod.hpp"
#include <unordered_map>
#include <string>

namespace iw {
namespace Audio {
	class AudioSpace {
	private:
		FMOD::System* m_system;

		FMOD::Channel* m_channel;
		int m_channels;
		float m_volume;

		std::unordered_map<std::string, FMOD::Sound*> m_sounds;
		std::string m_rootDir;

	public:
		IWAUDIO_API
		AudioSpace(
			int channels,
			std::string rootDir = "",
			float volume = 1.0f);

		IWAUDIO_API
		int Initialize();

		IWAUDIO_API
		void SetVolume(
			float volume);

		IWAUDIO_API
		float& GetVolume();

		IWAUDIO_API
		void CreateSound(
			std::string name);

		IWAUDIO_API
		void PlaySound(
			std::string name);
	private:
		bool CheckError(
			FMOD_RESULT result,
			const char* message);
	};
}

	using namespace Audio;
}
