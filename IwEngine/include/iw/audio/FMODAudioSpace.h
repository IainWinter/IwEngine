#pragma once

#include "AudioSpace.h"
#include "fmod/fmod_studio.hpp"
#include "iw/log/logger.h"
#include <string>
#include <vector>
#include <unordered_map>

#undef CreateEvent

#ifdef IW_DEBUG
#	define CHECK_ERROR(stmt, n) if(CheckError(stmt, n)) return n;
#else
#	define CHECK_ERROR(stmt, n) stmt
#endif

namespace iw
{
namespace Audio
{
	FMOD_RESULT __dontlookatme__RemoveInstanceFromList(
		FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
		FMOD_STUDIO_EVENTINSTANCE* event,
		void* parameters);

	class FMODAudioSpace
		: public IAudioSpace
	{
	private:
		FMOD::Studio::System* m_system;

		std::unordered_map<std::string, int> m_loaded; // name, handle - hashed paths of loaded objects

		std::unordered_map<int, FMOD::Studio::Bank*> m_banks;
		std::unordered_map<int, FMOD::Studio::EventDescription*> m_events;
		std::vector<FMOD::Studio::EventInstance*> m_instances;

		friend FMOD_RESULT iw::Audio::__dontlookatme__RemoveInstanceFromList(
			FMOD_STUDIO_EVENT_CALLBACK_TYPE,
			FMOD_STUDIO_EVENTINSTANCE*,
			void*);

	public:

		IWAUDIO_API
		int Initialize() override
		{
			LOG_INFO << "Initializing audio space using FMOD";

			CHECK_ERROR(
				FMOD::Studio::System::create(&m_system),
				ENGINE_FAILED_CREATE
			);

			// can channels be expanded past 32?

			CHECK_ERROR(
				m_system->initialize(32, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr),
				ENGINE_FAILED_INIT
			);

			return ENGINE_OK;
		}

		IWAUDIO_API
		int Update() override
		{
			CHECK_ERROR(
				m_system->update(),
				ENGINE_FAILED_UPDATE
			);

			return ENGINE_OK;
		}

		IWAUDIO_API
		int Destroy() override
		{
			// find how to destroy

			return ENGINE_OK;
		}

		IWAUDIO_API
		int Load(
			const std::string& path) override
		{
			// this could be loaded a bank or an event or a sound
			// depending on what path is, could add 

			std::string p = RootDirectory + path;

			if (IsLoaded(path))
			{
				return ENGINE_ALREADY_LOADED;
			}

			FMOD::Studio::Bank* bank;

			CHECK_ERROR(
				m_system->loadBankFile(p.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank),
				ENGINE_FAILED_LOAD_BANK
			);

			PutBank(path, bank);

			// enumerate all events

			int count;
			CHECK_ERROR(
				bank->getEventCount(&count),
				ENGINE_FAILED_LOAD_BANK
			);

			if (count > 0)
			{
				FMOD::Studio::EventDescription** events = new FMOD::Studio::EventDescription*[count];
				int _;

				CHECK_ERROR(
					bank->getEventList(events, count, &_),
					ENGINE_FAILED_LOAD_BANK
				);

				for (int i = 0; i < count; i++)
				{
					std::string eventName(1024, '\0');

					CHECK_ERROR(
						events[0]->getPath(eventName.data(), 1024, &_),
						ENGINE_FAILED_LOAD_BANK
					);

					PutEvent(eventName, events[i]);
				}

				delete[] events;
			}
		}

		IWAUDIO_API int Play(const std::string& path, bool loop, bool stream)  override { return ENGINE_OK; }
		IWAUDIO_API int Set(int handle, const std::string& parameter, float  value)  override { return ENGINE_OK; }
		IWAUDIO_API int Get(int handle, const std::string& parameter, float& value) override { return ENGINE_OK; }

		IWAUDIO_API int Start(int handle)  override { return ENGINE_OK; }
		IWAUDIO_API int Stop (int handle)  override { return ENGINE_OK; }
		IWAUDIO_API int Free (int handle)  override { return ENGINE_OK; }

		int SetVolume(float  volume)  override { return ENGINE_OK; }
		int GetVolume(float& volume)  override { return ENGINE_OK; }

		bool IsLoaded(
			const std::string& path)
		{
			return m_loaded.find(path) != m_loaded.end();
		}

	private:
		int MakeHandle(
			int type)
		{
			return SetHigh(m_loaded.size() + 1, type);
		}

		void PutLoaded(
			const std::string& path,
			int handle)
		{
			m_loaded.emplace(path, handle);
		}

		void PutBank(
			const std::string& path,
			FMOD::Studio::Bank* bank)
		{
			int handle = MakeHandle(1);
			PutLoaded(path, handle);
			m_banks[handle] = bank;
		}

		void PutEvent(
			const std::string& path,
			FMOD::Studio::EventDescription* event)
		{
			int handle = MakeHandle(2);
			PutLoaded(path, handle);
			m_events[handle] = event;
		}

		void PutInstance(const std::string& path) { PutLoaded(path, MakeHandle(3)); }

		bool CheckError(
			int result,
			AudioErrorCode code) override
		{
			static std::unordered_map<int, std::string> FMODtranslation;
			if (FMODtranslation.size() == 0)
			{
				FMODtranslation.emplace(FMOD_OK,                            "FMOD_OK");
				FMODtranslation.emplace(FMOD_ERR_BADCOMMAND,                "FMOD_ERR_BADCOMMAND");
				FMODtranslation.emplace(FMOD_ERR_CHANNEL_ALLOC,             "FMOD_ERR_CHANNEL_ALLOC");
				FMODtranslation.emplace(FMOD_ERR_CHANNEL_STOLEN,            "FMOD_ERR_CHANNEL_STOLEN");
				FMODtranslation.emplace(FMOD_ERR_DMA,                       "FMOD_ERR_DMA");
				FMODtranslation.emplace(FMOD_ERR_DSP_CONNECTION,            "FMOD_ERR_DSP_CONNECTION");
				FMODtranslation.emplace(FMOD_ERR_DSP_DONTPROCESS,           "FMOD_ERR_DSP_DONTPROCESS");
				FMODtranslation.emplace(FMOD_ERR_DSP_FORMAT,                "FMOD_ERR_DSP_FORMAT");
				FMODtranslation.emplace(FMOD_ERR_DSP_INUSE,                 "FMOD_ERR_DSP_INUSE");
				FMODtranslation.emplace(FMOD_ERR_DSP_NOTFOUND,              "FMOD_ERR_DSP_NOTFOUND");
				FMODtranslation.emplace(FMOD_ERR_DSP_RESERVED,              "FMOD_ERR_DSP_RESERVED");
				FMODtranslation.emplace(FMOD_ERR_DSP_SILENCE,               "FMOD_ERR_DSP_SILENCE");
				FMODtranslation.emplace(FMOD_ERR_DSP_TYPE,                  "FMOD_ERR_DSP_TYPE");
				FMODtranslation.emplace(FMOD_ERR_FILE_BAD,                  "FMOD_ERR_FILE_BAD");
				FMODtranslation.emplace(FMOD_ERR_FILE_COULDNOTSEEK,         "FMOD_ERR_FILE_COULDNOTSEEK");
				FMODtranslation.emplace(FMOD_ERR_FILE_DISKEJECTED,          "FMOD_ERR_FILE_DISKEJECTED");
				FMODtranslation.emplace(FMOD_ERR_FILE_EOF,                  "FMOD_ERR_FILE_EOF");
				FMODtranslation.emplace(FMOD_ERR_FILE_ENDOFDATA,            "FMOD_ERR_FILE_ENDOFDATA");
				FMODtranslation.emplace(FMOD_ERR_FILE_NOTFOUND,             "FMOD_ERR_FILE_NOTFOUND");
				FMODtranslation.emplace(FMOD_ERR_FORMAT,                    "FMOD_ERR_FORMAT");
				FMODtranslation.emplace(FMOD_ERR_HEADER_MISMATCH,           "FMOD_ERR_HEADER_MISMATCH");
				FMODtranslation.emplace(FMOD_ERR_HTTP,                      "FMOD_ERR_HTTP");
				FMODtranslation.emplace(FMOD_ERR_HTTP_ACCESS,               "FMOD_ERR_HTTP_ACCESS");
				FMODtranslation.emplace(FMOD_ERR_HTTP_PROXY_AUTH,           "FMOD_ERR_HTTP_PROXY_AUTH");
				FMODtranslation.emplace(FMOD_ERR_HTTP_SERVER_ERROR,         "FMOD_ERR_HTTP_SERVER_ERROR");
				FMODtranslation.emplace(FMOD_ERR_HTTP_TIMEOUT,              "FMOD_ERR_HTTP_TIMEOUT");
				FMODtranslation.emplace(FMOD_ERR_INITIALIZATION,            "FMOD_ERR_INITIALIZATION");
				FMODtranslation.emplace(FMOD_ERR_INITIALIZED,               "FMOD_ERR_INITIALIZED");
				FMODtranslation.emplace(FMOD_ERR_INTERNAL,                  "FMOD_ERR_INTERNAL");
				FMODtranslation.emplace(FMOD_ERR_INVALID_FLOAT,             "FMOD_ERR_INVALID_FLOAT");
				FMODtranslation.emplace(FMOD_ERR_INVALID_HANDLE,            "FMOD_ERR_INVALID_HANDLE");
				FMODtranslation.emplace(FMOD_ERR_INVALID_PARAM,             "FMOD_ERR_INVALID_PARAM");
				FMODtranslation.emplace(FMOD_ERR_INVALID_POSITION,          "FMOD_ERR_INVALID_POSITION");
				FMODtranslation.emplace(FMOD_ERR_INVALID_SPEAKER,           "FMOD_ERR_INVALID_SPEAKER");
				FMODtranslation.emplace(FMOD_ERR_INVALID_SYNCPOINT,         "FMOD_ERR_INVALID_SYNCPOINT");
				FMODtranslation.emplace(FMOD_ERR_INVALID_THREAD,            "FMOD_ERR_INVALID_THREAD");
				FMODtranslation.emplace(FMOD_ERR_INVALID_VECTOR,            "FMOD_ERR_INVALID_VECTOR");
				FMODtranslation.emplace(FMOD_ERR_MAXAUDIBLE,                "FMOD_ERR_MAXAUDIBLE");
				FMODtranslation.emplace(FMOD_ERR_MEMORY,                    "FMOD_ERR_MEMORY");
				FMODtranslation.emplace(FMOD_ERR_MEMORY_CANTPOINT,          "FMOD_ERR_MEMORY_CANTPOINT");
				FMODtranslation.emplace(FMOD_ERR_NEEDS3D,                   "FMOD_ERR_NEEDS3D");
				FMODtranslation.emplace(FMOD_ERR_NEEDSHARDWARE,             "FMOD_ERR_NEEDSHARDWARE");
				FMODtranslation.emplace(FMOD_ERR_NET_CONNECT,               "FMOD_ERR_NET_CONNECT");
				FMODtranslation.emplace(FMOD_ERR_NET_SOCKET_ERROR,          "FMOD_ERR_NET_SOCKET_ERROR");
				FMODtranslation.emplace(FMOD_ERR_NET_URL,                   "FMOD_ERR_NET_URL");
				FMODtranslation.emplace(FMOD_ERR_NET_WOULD_BLOCK,           "FMOD_ERR_NET_WOULD_BLOCK");
				FMODtranslation.emplace(FMOD_ERR_NOTREADY,                  "FMOD_ERR_NOTREADY");
				FMODtranslation.emplace(FMOD_ERR_OUTPUT_ALLOCATED,          "FMOD_ERR_OUTPUT_ALLOCATED");
				FMODtranslation.emplace(FMOD_ERR_OUTPUT_CREATEBUFFER,       "FMOD_ERR_OUTPUT_CREATEBUFFER");
				FMODtranslation.emplace(FMOD_ERR_OUTPUT_DRIVERCALL,         "FMOD_ERR_OUTPUT_DRIVERCALL");
				FMODtranslation.emplace(FMOD_ERR_OUTPUT_FORMAT,             "FMOD_ERR_OUTPUT_FORMAT");
				FMODtranslation.emplace(FMOD_ERR_OUTPUT_INIT,               "FMOD_ERR_OUTPUT_INIT");
				FMODtranslation.emplace(FMOD_ERR_OUTPUT_NODRIVERS,          "FMOD_ERR_OUTPUT_NODRIVERS");
				FMODtranslation.emplace(FMOD_ERR_PLUGIN,                    "FMOD_ERR_PLUGIN");
				FMODtranslation.emplace(FMOD_ERR_PLUGIN_MISSING,            "FMOD_ERR_PLUGIN_MISSING");
				FMODtranslation.emplace(FMOD_ERR_PLUGIN_RESOURCE,           "FMOD_ERR_PLUGIN_RESOURCE");
				FMODtranslation.emplace(FMOD_ERR_PLUGIN_VERSION,            "FMOD_ERR_PLUGIN_VERSION");
				FMODtranslation.emplace(FMOD_ERR_RECORD,                    "FMOD_ERR_RECORD");
				FMODtranslation.emplace(FMOD_ERR_REVERB_CHANNELGROUP,       "FMOD_ERR_REVERB_CHANNELGROUP");
				FMODtranslation.emplace(FMOD_ERR_REVERB_INSTANCE,           "FMOD_ERR_REVERB_INSTANCE");
				FMODtranslation.emplace(FMOD_ERR_SUBSOUNDS,                 "FMOD_ERR_SUBSOUNDS");
				FMODtranslation.emplace(FMOD_ERR_SUBSOUND_ALLOCATED,        "FMOD_ERR_SUBSOUND_ALLOCATED");
				FMODtranslation.emplace(FMOD_ERR_SUBSOUND_CANTMOVE,         "FMOD_ERR_SUBSOUND_CANTMOVE");
				FMODtranslation.emplace(FMOD_ERR_TAGNOTFOUND,               "FMOD_ERR_TAGNOTFOUND");
				FMODtranslation.emplace(FMOD_ERR_TOOMANYCHANNELS,           "FMOD_ERR_TOOMANYCHANNELS");
				FMODtranslation.emplace(FMOD_ERR_TRUNCATED,                 "FMOD_ERR_TRUNCATED");
				FMODtranslation.emplace(FMOD_ERR_UNIMPLEMENTED,             "FMOD_ERR_UNIMPLEMENTED");
				FMODtranslation.emplace(FMOD_ERR_UNINITIALIZED,             "FMOD_ERR_UNINITIALIZED");
				FMODtranslation.emplace(FMOD_ERR_UNSUPPORTED,               "FMOD_ERR_UNSUPPORTED");
				FMODtranslation.emplace(FMOD_ERR_VERSION,                   "FMOD_ERR_VERSION");
				FMODtranslation.emplace(FMOD_ERR_EVENT_ALREADY_LOADED,      "FMOD_ERR_EVENT_ALREADY_LOADED");
				FMODtranslation.emplace(FMOD_ERR_EVENT_LIVEUPDATE_BUSY,     "FMOD_ERR_EVENT_LIVEUPDATE_BUSY");
				FMODtranslation.emplace(FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH, "FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH");
				FMODtranslation.emplace(FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT,  "FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT");
				FMODtranslation.emplace(FMOD_ERR_EVENT_NOTFOUND,            "FMOD_ERR_EVENT_NOTFOUND");
				FMODtranslation.emplace(FMOD_ERR_STUDIO_UNINITIALIZED,      "FMOD_ERR_STUDIO_UNINITIALIZED");
				FMODtranslation.emplace(FMOD_ERR_STUDIO_NOT_LOADED,         "FMOD_ERR_STUDIO_NOT_LOADED");
				FMODtranslation.emplace(FMOD_ERR_INVALID_STRING,            "FMOD_ERR_INVALID_STRING");
				FMODtranslation.emplace(FMOD_ERR_ALREADY_LOCKED,            "FMOD_ERR_ALREADY_LOCKED");
				FMODtranslation.emplace(FMOD_ERR_NOT_LOCKED,                "FMOD_ERR_NOT_LOCKED");
				FMODtranslation.emplace(FMOD_ERR_RECORD_DISCONNECTED,       "FMOD_ERR_RECORD_DISCONNECTED");
				FMODtranslation.emplace(FMOD_ERR_TOOMANYSAMPLES,            "FMOD_ERR_TOOMANYSAMPLES");
			}

			static std::unordered_map<int, std::string> translation;
			if (translation.size() == 0)
			{
				translation.emplace(ENGINE_OK,                "Engine is ok");
				translation.emplace(ENGINE_FAILED_CREATE,     "Engine failed to create fmod system");
				translation.emplace(ENGINE_FAILED_INIT,       "Engine failed to init fmod system");
				translation.emplace(ENGINE_FAILED_UPDATE,     "Engine failed to update fmod system");
				translation.emplace(ENGINE_FAILED_LOAD_BANK,  "Engine failed to load a bank");
				translation.emplace(ENGINE_ALREADY_LOADED,    "Engine tried to load a bank that was already loaded");
			}

			if (result != FMOD_OK) {
				LOG_ERROR << "Fmod error code: " << FMODtranslation.at(result) << "(" << result << "). "
						<< "\n\tEngine message: " << translation.at(code);
		
				return true;
			}

			return false;
		}

	//	IWAUDIO_API
	//	AudioSpaceStudio(
	//		std::string rootDir = "",
	//		int channels = 32,
	//		float volume = 1.0f);

	//	IWAUDIO_API
	//	~AudioSpaceStudio();

	//	IWAUDIO_API
	//	int Initialize() override;

	//	IWAUDIO_API
	//	void Update() override;

	//	IWAUDIO_API
	//	void LoadBank(
	//		std::string name);

	//	IWAUDIO_API
	//	void CreateEvent(
	//		std::string name);

	//	IWAUDIO_API
	//	int CreateInstance(
	//		std::string name,
	//		bool realeaseAtEnd = true);

	//	IWAUDIO_API
	//	void SetInstanceParameter(
	//		int instance,
	//		std::string name,
	//		float value);

	//	IWAUDIO_API
	//	void RemoveInstance(
	//		int index);

	//	IWAUDIO_API
	//	void StartInstance(
	//		int index);

	//	IWAUDIO_API
	//	void StopInstance(
	//		int index);

	//	IWAUDIO_API
	//	void SetVolume(
	//		float volume) override;
	//private:
	//	IWAUDIO_API
	//	void RemoveInstance(
	//		FMOD::Studio::EventInstance* instance);
	};
}

	using namespace Audio;
}

#undef CHECK_ERROR
