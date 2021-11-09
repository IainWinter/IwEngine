//#include "iw/audio/AudioSpace.h"
//#include "iw/log/logger.h"
//#include "fmod/fmod.h"
//#include <unordered_map>
//
//namespace iw {
//namespace Audio {
//	AudioSpace::AudioSpace(
//		std::string rootDir,
//		int channels,
//		float volume)
//		: m_rootDir(rootDir)
//		, m_channels(channels)
//		, m_volume(volume)
//	{}
//
//	AudioSpaceRaw* AudioSpace::AsRaw() {
//		return (AudioSpaceRaw*)this; // use dynamic cast
//	}
//
//	AudioSpaceStudio* AudioSpace::AsStudio() {
//		return (AudioSpaceStudio*)this; // use dynamic cast
//	}
//
//	std::string& AudioSpace::GetRootDir() {
//		return m_rootDir;
//	}
//
//	int AudioSpace::GetChannels() {
//		return m_channels;
//	}
//
//	float& AudioSpace::GetVolume() {
//		return m_volume;
//	}
//
//	void AudioSpace::SetRootDir(
//		std::string& rootDir)
//	{
//		m_rootDir = rootDir;
//	}
//
//	void AudioSpace::SetChannels(
//		int channels)
//	{
//		m_channels = channels;
//	}
//
//	void AudioSpace::SetVolume(
//		float volume)
//	{
//		m_volume = volume;
//	}
//
//	bool AudioSpace::CheckError(
//		int result,
//		std::string message)
//	{
//		static std::unordered_map<int, std::string> translation;
//		if (translation.size() == 0)
//		{
//            translation.emplace(FMOD_OK,                            "FMOD_OK");
//            translation.emplace(FMOD_ERR_BADCOMMAND,                "FMOD_ERR_BADCOMMAND");
//            translation.emplace(FMOD_ERR_CHANNEL_ALLOC,             "FMOD_ERR_CHANNEL_ALLOC");
//            translation.emplace(FMOD_ERR_CHANNEL_STOLEN,            "FMOD_ERR_CHANNEL_STOLEN");
//            translation.emplace(FMOD_ERR_DMA,                       "FMOD_ERR_DMA");
//            translation.emplace(FMOD_ERR_DSP_CONNECTION,            "FMOD_ERR_DSP_CONNECTION");
//            translation.emplace(FMOD_ERR_DSP_DONTPROCESS,           "FMOD_ERR_DSP_DONTPROCESS");
//            translation.emplace(FMOD_ERR_DSP_FORMAT,                "FMOD_ERR_DSP_FORMAT");
//            translation.emplace(FMOD_ERR_DSP_INUSE,                 "FMOD_ERR_DSP_INUSE");
//            translation.emplace(FMOD_ERR_DSP_NOTFOUND,              "FMOD_ERR_DSP_NOTFOUND");
//            translation.emplace(FMOD_ERR_DSP_RESERVED,              "FMOD_ERR_DSP_RESERVED");
//            translation.emplace(FMOD_ERR_DSP_SILENCE,               "FMOD_ERR_DSP_SILENCE");
//            translation.emplace(FMOD_ERR_DSP_TYPE,                  "FMOD_ERR_DSP_TYPE");
//            translation.emplace(FMOD_ERR_FILE_BAD,                  "FMOD_ERR_FILE_BAD");
//            translation.emplace(FMOD_ERR_FILE_COULDNOTSEEK,         "FMOD_ERR_FILE_COULDNOTSEEK");
//            translation.emplace(FMOD_ERR_FILE_DISKEJECTED,          "FMOD_ERR_FILE_DISKEJECTED");
//            translation.emplace(FMOD_ERR_FILE_EOF,                  "FMOD_ERR_FILE_EOF");
//            translation.emplace(FMOD_ERR_FILE_ENDOFDATA,            "FMOD_ERR_FILE_ENDOFDATA");
//            translation.emplace(FMOD_ERR_FILE_NOTFOUND,             "FMOD_ERR_FILE_NOTFOUND");
//            translation.emplace(FMOD_ERR_FORMAT,                    "FMOD_ERR_FORMAT");
//            translation.emplace(FMOD_ERR_HEADER_MISMATCH,           "FMOD_ERR_HEADER_MISMATCH");
//            translation.emplace(FMOD_ERR_HTTP,                      "FMOD_ERR_HTTP");
//            translation.emplace(FMOD_ERR_HTTP_ACCESS,               "FMOD_ERR_HTTP_ACCESS");
//            translation.emplace(FMOD_ERR_HTTP_PROXY_AUTH,           "FMOD_ERR_HTTP_PROXY_AUTH");
//            translation.emplace(FMOD_ERR_HTTP_SERVER_ERROR,         "FMOD_ERR_HTTP_SERVER_ERROR");
//            translation.emplace(FMOD_ERR_HTTP_TIMEOUT,              "FMOD_ERR_HTTP_TIMEOUT");
//            translation.emplace(FMOD_ERR_INITIALIZATION,            "FMOD_ERR_INITIALIZATION");
//            translation.emplace(FMOD_ERR_INITIALIZED,               "FMOD_ERR_INITIALIZED");
//            translation.emplace(FMOD_ERR_INTERNAL,                  "FMOD_ERR_INTERNAL");
//            translation.emplace(FMOD_ERR_INVALID_FLOAT,             "FMOD_ERR_INVALID_FLOAT");
//            translation.emplace(FMOD_ERR_INVALID_HANDLE,            "FMOD_ERR_INVALID_HANDLE");
//            translation.emplace(FMOD_ERR_INVALID_PARAM,             "FMOD_ERR_INVALID_PARAM");
//            translation.emplace(FMOD_ERR_INVALID_POSITION,          "FMOD_ERR_INVALID_POSITION");
//            translation.emplace(FMOD_ERR_INVALID_SPEAKER,           "FMOD_ERR_INVALID_SPEAKER");
//            translation.emplace(FMOD_ERR_INVALID_SYNCPOINT,         "FMOD_ERR_INVALID_SYNCPOINT");
//            translation.emplace(FMOD_ERR_INVALID_THREAD,            "FMOD_ERR_INVALID_THREAD");
//            translation.emplace(FMOD_ERR_INVALID_VECTOR,            "FMOD_ERR_INVALID_VECTOR");
//            translation.emplace(FMOD_ERR_MAXAUDIBLE,                "FMOD_ERR_MAXAUDIBLE");
//            translation.emplace(FMOD_ERR_MEMORY,                    "FMOD_ERR_MEMORY");
//            translation.emplace(FMOD_ERR_MEMORY_CANTPOINT,          "FMOD_ERR_MEMORY_CANTPOINT");
//            translation.emplace(FMOD_ERR_NEEDS3D,                   "FMOD_ERR_NEEDS3D");
//            translation.emplace(FMOD_ERR_NEEDSHARDWARE,             "FMOD_ERR_NEEDSHARDWARE");
//            translation.emplace(FMOD_ERR_NET_CONNECT,               "FMOD_ERR_NET_CONNECT");
//            translation.emplace(FMOD_ERR_NET_SOCKET_ERROR,          "FMOD_ERR_NET_SOCKET_ERROR");
//            translation.emplace(FMOD_ERR_NET_URL,                   "FMOD_ERR_NET_URL");
//            translation.emplace(FMOD_ERR_NET_WOULD_BLOCK,           "FMOD_ERR_NET_WOULD_BLOCK");
//            translation.emplace(FMOD_ERR_NOTREADY,                  "FMOD_ERR_NOTREADY");
//            translation.emplace(FMOD_ERR_OUTPUT_ALLOCATED,          "FMOD_ERR_OUTPUT_ALLOCATED");
//            translation.emplace(FMOD_ERR_OUTPUT_CREATEBUFFER,       "FMOD_ERR_OUTPUT_CREATEBUFFER");
//            translation.emplace(FMOD_ERR_OUTPUT_DRIVERCALL,         "FMOD_ERR_OUTPUT_DRIVERCALL");
//            translation.emplace(FMOD_ERR_OUTPUT_FORMAT,             "FMOD_ERR_OUTPUT_FORMAT");
//            translation.emplace(FMOD_ERR_OUTPUT_INIT,               "FMOD_ERR_OUTPUT_INIT");
//            translation.emplace(FMOD_ERR_OUTPUT_NODRIVERS,          "FMOD_ERR_OUTPUT_NODRIVERS");
//            translation.emplace(FMOD_ERR_PLUGIN,                    "FMOD_ERR_PLUGIN");
//            translation.emplace(FMOD_ERR_PLUGIN_MISSING,            "FMOD_ERR_PLUGIN_MISSING");
//            translation.emplace(FMOD_ERR_PLUGIN_RESOURCE,           "FMOD_ERR_PLUGIN_RESOURCE");
//            translation.emplace(FMOD_ERR_PLUGIN_VERSION,            "FMOD_ERR_PLUGIN_VERSION");
//            translation.emplace(FMOD_ERR_RECORD,                    "FMOD_ERR_RECORD");
//            translation.emplace(FMOD_ERR_REVERB_CHANNELGROUP,       "FMOD_ERR_REVERB_CHANNELGROUP");
//            translation.emplace(FMOD_ERR_REVERB_INSTANCE,           "FMOD_ERR_REVERB_INSTANCE");
//            translation.emplace(FMOD_ERR_SUBSOUNDS,                 "FMOD_ERR_SUBSOUNDS");
//            translation.emplace(FMOD_ERR_SUBSOUND_ALLOCATED,        "FMOD_ERR_SUBSOUND_ALLOCATED");
//            translation.emplace(FMOD_ERR_SUBSOUND_CANTMOVE,         "FMOD_ERR_SUBSOUND_CANTMOVE");
//            translation.emplace(FMOD_ERR_TAGNOTFOUND,               "FMOD_ERR_TAGNOTFOUND");
//            translation.emplace(FMOD_ERR_TOOMANYCHANNELS,           "FMOD_ERR_TOOMANYCHANNELS");
//            translation.emplace(FMOD_ERR_TRUNCATED,                 "FMOD_ERR_TRUNCATED");
//            translation.emplace(FMOD_ERR_UNIMPLEMENTED,             "FMOD_ERR_UNIMPLEMENTED");
//            translation.emplace(FMOD_ERR_UNINITIALIZED,             "FMOD_ERR_UNINITIALIZED");
//            translation.emplace(FMOD_ERR_UNSUPPORTED,               "FMOD_ERR_UNSUPPORTED");
//            translation.emplace(FMOD_ERR_VERSION,                   "FMOD_ERR_VERSION");
//            translation.emplace(FMOD_ERR_EVENT_ALREADY_LOADED,      "FMOD_ERR_EVENT_ALREADY_LOADED");
//            translation.emplace(FMOD_ERR_EVENT_LIVEUPDATE_BUSY,     "FMOD_ERR_EVENT_LIVEUPDATE_BUSY");
//            translation.emplace(FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH, "FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH");
//            translation.emplace(FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT,  "FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT");
//            translation.emplace(FMOD_ERR_EVENT_NOTFOUND,            "FMOD_ERR_EVENT_NOTFOUND");
//            translation.emplace(FMOD_ERR_STUDIO_UNINITIALIZED,      "FMOD_ERR_STUDIO_UNINITIALIZED");
//            translation.emplace(FMOD_ERR_STUDIO_NOT_LOADED,         "FMOD_ERR_STUDIO_NOT_LOADED");
//            translation.emplace(FMOD_ERR_INVALID_STRING,            "FMOD_ERR_INVALID_STRING");
//            translation.emplace(FMOD_ERR_ALREADY_LOCKED,            "FMOD_ERR_ALREADY_LOCKED");
//            translation.emplace(FMOD_ERR_NOT_LOCKED,                "FMOD_ERR_NOT_LOCKED");
//            translation.emplace(FMOD_ERR_RECORD_DISCONNECTED,       "FMOD_ERR_RECORD_DISCONNECTED");
//            translation.emplace(FMOD_ERR_TOOMANYSAMPLES,            "FMOD_ERR_TOOMANYSAMPLES");
//		}
//
//		if (result != FMOD_OK) {
//			LOG_ERROR << "Fmod error code: " << translation.at(result) << "(" << result << ")"
//				      << ". engine message: " << message;
//		
//			return true;
//		}
//
//		return false;
//	}
//}
//}
