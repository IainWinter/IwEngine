#pragma once

#include "IwAudio.h"
#include <string>

#undef PlaySound

namespace iw {
namespace Audio {
	class AudioSpaceRaw;
	class AudioSpaceStudio;

	class AudioSpace {
	private:
		std::string m_rootDir;
		int m_channels;
		float m_volume;

	public:
		IWAUDIO_API
		AudioSpace(
			std::string rootDir,
			int channels,
			float volume);

		IWAUDIO_API
		virtual ~AudioSpace() = default;

		virtual int Initialize() = 0;
		virtual void Update() = 0;

		IWAUDIO_API
		AudioSpaceRaw* AsRaw();

		IWAUDIO_API
		AudioSpaceStudio* AsStudio();

		IWAUDIO_API
		virtual std::string& GetRootDir();

		IWAUDIO_API
		virtual int GetChannels();

		IWAUDIO_API
		virtual float& GetVolume();

		IWAUDIO_API
		virtual void SetRootDir(
			std::string& rootDir);

		IWAUDIO_API
		virtual void SetChannels(
			int channels);

		IWAUDIO_API
		virtual void SetVolume(
			float volume);
	protected:
		bool CheckError(
			int result,
			std::string message);
	};
}

	using namespace Audio;
}
