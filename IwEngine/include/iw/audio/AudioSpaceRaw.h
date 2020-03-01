#pragma once

#include "AudioSpace.h"
#include "fmod/fmod.hpp"
#include <unordered_map>

#undef PlaySound

namespace iw {
namespace Audio {
	class AudioSpaceRaw
		: public AudioSpace
	{
	private:
		FMOD::System* m_system;
		FMOD::Channel* m_channel;

		std::unordered_map<std::string, FMOD::Sound*> m_sounds;

	public:
		IWAUDIO_API
		AudioSpaceRaw(
			std::string rootDir = "",
			int channels = 32,
			float volume = 1.0f);

		IWAUDIO_API
		int Initialize() override;

		IWAUDIO_API
		void Update() override;

		IWAUDIO_API
		void CreateSound(
			std::string name);

		IWAUDIO_API
		void PlaySound(
			std::string name);
	};
}

	using namespace Audio;
}
