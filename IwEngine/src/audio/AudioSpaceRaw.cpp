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

	int AudioSpaceRaw::Initialize() {
		FMOD_RESULT error;

		error = FMOD::System_Create(&m_system);
		CHECK_ERROR(error, "Audio space did not get created!", 1);

		error = m_system->init(GetChannels(), FMOD_INIT_NORMAL, nullptr);
		CHECK_ERROR(error, "Audio space did not get initliaze!", 2);

		return 0;
	}

	void AudioSpaceRaw::Update() {
		m_system->update();
	}

	void AudioSpaceRaw::CreateSound(
		std::string name)
	{
		std::string path = GetRootDir() + name;

		auto itr = m_sounds.find(name);
		if (itr != m_sounds.end()) {
			itr->second->release();
			delete itr->second; // ?
			itr->second = nullptr;
		}

		FMOD_RESULT error;
		error = m_system->createSound(path.c_str(), FMOD_DEFAULT, 0, &m_sounds[name]);
		CHECK_ERROR(error, "Sound did not get created!");
	}

	void AudioSpaceRaw::PlaySound(
		std::string name)
	{
		std::string path = GetRootDir() + name;

		auto itr = m_sounds.find(name);
		if (itr == m_sounds.end()) {
			LOG_INFO << "Could not find sound!";
			return;
		}

		FMOD_RESULT error;
		error = m_system->playSound(itr->second, 0, false, &m_channel);
		CHECK_ERROR(error, "Sound did not get played!");

		m_channel->setVolume(GetVolume());
	}
}
}
