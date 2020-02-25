#include "iw/audio/AudioSpace.h"
#include "iw/log/logger.h"

#define CHECK_ERROR(err, msg, n) if(CheckError(err, msg)) return n;

namespace iw {
namespace Audio {
	AudioSpace::AudioSpace(
		int channels,
		std::string rootDir,
		float volume)
		: m_system(nullptr)
		, m_channel(nullptr)
		, m_channels(channels)
		, m_volume(volume)
		, m_rootDir(rootDir)
	{
		FMOD_RESULT error;

		error = FMOD::System_Create(&m_system);
		CHECK_ERROR(error, "Audio space did not get created!");
	}

	int AudioSpace::Initialize() {
		FMOD_RESULT error;

		error = m_system->init(m_channels, FMOD_INIT_NORMAL, nullptr);
		CHECK_ERROR(error, "Audio space did not get initliaze!", 1);

		return 0;
	}

	void AudioSpace::SetVolume(
		float volume)
	{
		m_volume = volume;
	}

	float& AudioSpace::GetVolume() {
		return m_volume;
	}

	void AudioSpace::CreateSound(
		std::string name)
	{
		std::string path = m_rootDir + name;

		auto itr = m_sounds.find(path);
		if (itr != m_sounds.end()) {
			itr->second->release();
		}

		FMOD_RESULT error;
		error = m_system->createSound(path.c_str(), FMOD_DEFAULT, 0, &m_sounds[path]);
		CHECK_ERROR(error, "Sound did not get created!");
	}

	void AudioSpace::PlaySound(
		std::string name)
	{
		std::string path = m_rootDir + name;

		auto itr = m_sounds.find(path);
		if (itr == m_sounds.end()) {
			LOG_INFO << "Could not find sound!";
			return;
		}


		FMOD_RESULT error;
		error = m_system->playSound(itr->second, 0, false, &m_channel);
		CHECK_ERROR(error, "Sound did not get played!");
		m_channel->setVolume(m_volume);
	}

	bool AudioSpace::CheckError(
		FMOD_RESULT result,
		const char* message)
	{
		if (result != FMOD_OK) {
			LOG_ERROR << message;
		
			return true;
		}

		return false;
	}
}
}
