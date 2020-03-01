#include "iw/audio/AudioSpace.h"
#include "iw/log/logger.h"
#include "fmod/fmod.h"

namespace iw {
namespace Audio {
	AudioSpace::AudioSpace(
		std::string rootDir,
		int channels,
		float volume)
		: m_rootDir(rootDir)
		, m_channels(channels)
		, m_volume(volume)
	{}

	AudioSpaceRaw* AudioSpace::AsRaw() {
		return (AudioSpaceRaw*)this;
	}

	AudioSpaceStudio* AudioSpace::AsStudio() {
		return (AudioSpaceStudio*)this;
	}

	std::string& AudioSpace::GetRootDir() {
		return m_rootDir;
	}

	int AudioSpace::GetChannels() {
		return m_channels;
	}

	float& AudioSpace::GetVolume() {
		return m_volume;
	}

	void AudioSpace::SetRootDir(
		std::string& rootDir)
	{
		m_rootDir = rootDir;
	}

	void AudioSpace::SetChannels(
		int channels)
	{
		m_channels = channels;
	}

	void AudioSpace::SetVolume(
		float volume)
	{
		m_volume = volume;
	}

	bool AudioSpace::CheckError(
		int result,
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
