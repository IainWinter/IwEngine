#include "iw/audio/AudioSpaceRaw.h"
#include "iw/log/logger.h"
#include "fmod/fmod.hpp"
#include "CHECK_ERROR.h"

namespace iw {
namespace Audio {
	AudioSpaceRaw::AudioSpaceRaw(
		std::string rootDir,
		int channels,
		float volume)
		: AudioSpace(rootDir, channels, volume)
		, m_system(nullptr)
		, m_channel(nullptr)
	{}

	AudioSpaceRaw::~AudioSpaceRaw() {
		for (auto itr : m_sounds) {
			itr.second->release();
		}

		m_system->release();
	}

	int AudioSpaceRaw::Initialize() {
		CHECK_ERROR(
			FMOD::System_Create(&m_system),
			"Audio space did not get created!", 1
		);
		
		CHECK_ERROR(
			m_system->init(GetChannels(), FMOD_INIT_NORMAL, nullptr),
			"Audio space did not get initliaze!", 2
		);

		return 0;
	}

	void AudioSpaceRaw::Update() {
		CHECK_ERROR(
			m_system->update(),
			"Audio space could failed to update!"
		);
	}

	void AudioSpaceRaw::CreateSound(
		std::string name)
	{
		std::string path = GetRootDir() + name;

		auto itr = m_sounds.find(name);
		if (itr != m_sounds.end()) {
			itr->second->release();
		}

		CHECK_ERROR(
			m_system->createSound(path.c_str(), FMOD_DEFAULT, 0, &m_sounds[name]),
			"Sound did not get created!"
		);
	}

	void AudioSpaceRaw::PlaySound(
		std::string name)
	{
		std::string path = GetRootDir() + name;

		auto itr = m_sounds.find(name);
#ifdef IW_DEBUG
		if (itr != m_sounds.end()) {
			LOG_WARNING << "Could not find sound " << name << "!";
			return;
		}
#endif
		CHECK_ERROR(
			m_system->playSound(itr->second, 0, false, &m_channel),
			"Sound " + name + "did not get played!"
		);

		CHECK_ERROR(
			m_channel->setVolume(GetVolume());
			"Volume could not be set!"
		);
	}
}
}
