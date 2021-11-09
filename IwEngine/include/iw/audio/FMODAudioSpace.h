#pragma once

#include "AudioSpace.h"
#include "fmod/fmod_studio.hpp"

#undef CreateEvent

#ifdef IW_DEBUG
#	define CHECK_ERROR(stmt, n) if(CheckError(stmt, n)) return n;
#else
#	define CHECK_ERROR(stmt, n) stmt
#endif

#define H_BANK 1
#define H_EVENT 2
#define H_INSTANCE 3
#define H_BUS 4
#define H_VCA 5

#define CHECK_HANDLE(h) if (!IsLoaded(h)) return log(ENGINE_FAILED_HANDLE_NOT_LOADED);

namespace iw
{
namespace Audio
{
	class FMODAudioSpace
		: public IAudioSpace
	{
	private:
		FMOD::Studio::System* m_system;

		std::unordered_map<int, FMOD::Studio::Bank*>             m_banks;
		std::unordered_map<int, FMOD::Studio::Bus*>              m_buses;
		std::unordered_map<int, FMOD::Studio::VCA*>              m_vcas;
		std::unordered_map<int, FMOD::Studio::EventDescription*> m_events;
		std::unordered_map<int, FMOD::Studio::EventInstance*>    m_instances;

		using InstanceUserData = std::pair<int, FMODAudioSpace*>;

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

		// Loads a bank and it's event descriptions
		IWAUDIO_API
		int Load(
			const std::string& path) override
		{
			// this could be loaded a bank or an event or a sound
			// depending on what path is, could add 

			std::string p = RootDirectory + path;

			if (IsLoaded(path))
			{
				return log(ENGINE_ALREADY_LOADED);
			}

			FMOD::Studio::Bank* bank;

			CHECK_ERROR(
				m_system->loadBankFile(p.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank),
				ENGINE_FAILED_LOAD_BANK
			);

			int handle = Put(path, bank);

			// enumerate all items in bank
			// not sure if this is needed?

			using namespace FMOD::Studio;

			LoadFromBank<EventDescription>(
				bank,
				[](Bank* bank, int& count) {
					return bank->getEventCount(&count);
				},
				[](Bank* bank, EventDescription** items, int count) {
					int _;
					return bank->getEventList(items, count, &_);
				}
			),

			LoadFromBank<Bus>(
				bank,
				[](Bank* bank, int& count) {
					return bank->getBusCount(&count);
				},
				[](Bank* bank, Bus** items, int count) {
					int _;
					return bank->getBusList(items, count, &_);
				}
			),
			
			LoadFromBank<VCA>(
				bank,
				[](Bank* bank, int& count) {
					return bank->getVCACount(&count);
				},
				[](Bank* bank, VCA** items, int count) {
					int _;
					return bank->getVCAList(items, count, &_);
				}
			);

			return handle;
		}

		// if path has event:/ then play an event from a bank, else treat like a url and call playSound
		// return the instance handle
		IWAUDIO_API
		int Play(
			const std::string& path,
			bool loop,
			bool stream) override
		{
			if (path.find("event:/") == 0)
			{
				int eventHandle = GetHandle(path);

				if (eventHandle == 0)
				{
					return log(ENGINE_FAILED_LOAD_INSTANCE);
				}

				FMOD::Studio::EventInstance* instance;
				CHECK_ERROR(
					m_events.at(eventHandle)->createInstance(&instance),
					ENGINE_FAILED_MAKE_INSTANCE
				);

				FMOD_STUDIO_EVENT_CALLBACK callback = [](
					FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
					FMOD_STUDIO_EVENTINSTANCE* event,
					void* parameters)
				{
					auto [inst, me] = *(InstanceUserData*)parameters;
					me->m_instances.erase(inst);
					return FMOD_OK;
				};

				int instanceHandle = Put(path, instance);

				CHECK_ERROR(
					instance->setUserData(new InstanceUserData(instanceHandle, this)),
					ENGINE_FAILED_MAKE_INSTANCE
				);

				CHECK_ERROR(
					instance->setCallback(
						callback,
						FMOD_STUDIO_EVENT_CALLBACK_STOPPED
					),
					ENGINE_FAILED_MAKE_INSTANCE
				);

				Start(instanceHandle);

				return instanceHandle;
			}


			return ENGINE_OK;
		}

		// get and set parameters by string
		// fmod has volume and pitch as special functions
		// but for simplicity, I'll just pipe everything through these strings

		IWAUDIO_API
		int Set(
			int handle,
			const std::string& parameter,
			float value) override
		{
			CHECK_HANDLE(handle);

			if (GetType(handle) == H_INSTANCE)
			{
				CHECK_ERROR(
					m_instances[handle]->setParameterByName(parameter.c_str(), value),
					ENGINE_FAILED_SET_PARAM
				);
			}

			return ENGINE_OK;
		}

		IWAUDIO_API
		int Get(
			int handle,
			const std::string& parameter,
			float& value) override
		{
			CHECK_HANDLE(handle);

			if (GetType(handle) == H_INSTANCE)
			{
				CHECK_ERROR(
					m_instances[handle]->getParameterByName(parameter.c_str(), &value),
					ENGINE_FAILED_GET_PARAM
				);
			}

			return ENGINE_OK;
		}

		IWAUDIO_API
		int Start(
			int handle) override
		{
			CHECK_HANDLE(handle);

			if (GetType(handle) == H_INSTANCE)
			{
				m_instances[handle]->start();
				return ENGINE_OK;
			}

			return log(ENGINE_FAILED_INVALID_HANDLE); // dif flow than other with !!bottom should return ENGINE_OK!!
		}

		IWAUDIO_API
		int Stop(
			int handle) override
		{
			CHECK_HANDLE(handle);

			if (GetType(handle) == H_INSTANCE)
			{
				m_instances[handle]->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
				return ENGINE_OK;
			}

			return log(ENGINE_FAILED_INVALID_HANDLE);
		}

		IWAUDIO_API
		int Free(
			int handle) override
		{
			CHECK_HANDLE(handle);

			if (GetType (handle) == H_INSTANCE)
			{
				m_instances[handle]->release();
				return ENGINE_OK;
			}

			return log(ENGINE_FAILED_INVALID_HANDLE);
		}

		IWAUDIO_API
		int SetVolume(
			int handle,
			float volume) override
		{
			CHECK_HANDLE(handle);

			int type = GetType(handle);

			switch (type)
			{
				case H_INSTANCE: m_instances[handle]->setVolume(volume); break;
				case H_BUS:      m_buses    [handle]->setVolume(volume); break;
				case H_VCA:      m_vcas     [handle]->setVolume(volume); break;
				default:
				{
					return log(ENGINE_FAILED_INVALID_HANDLE);
				}
			}

			return ENGINE_OK;
		}

		IWAUDIO_API
		int GetVolume(
			int handle,
			float& volume) override
		{
			CHECK_HANDLE(handle);

			int type = GetType(handle);

			switch (type)
			{
				case H_INSTANCE: m_instances[handle]->getVolume(&volume); break;
				case H_BUS:      m_buses    [handle]->getVolume(&volume); break;
				case H_VCA:      m_vcas     [handle]->getVolume(&volume); break;
				default:
				{
					return log(ENGINE_FAILED_INVALID_HANDLE);
				}
			}

			return ENGINE_OK;
		}

		IWAUDIO_API
		bool IsLoaded(
			int handle) const override
		{
			switch (GetType(handle))
			{
				case H_BANK:     return m_banks    .find(handle) != m_banks    .end();
				case H_EVENT:    return m_events   .find(handle) != m_events   .end();
				case H_INSTANCE: return m_instances.find(handle) != m_instances.end();
				case H_BUS:      return m_buses    .find(handle) != m_buses    .end();
				case H_VCA:      return m_vcas     .find(handle) != m_vcas     .end();
			}

			return 0;
		}

	private:
		int Put(
			const std::string& path,
			FMOD::Studio::Bank* bank)
		{
			int handle = MakeHandle(H_BANK);
			PutLoaded(path, handle);
			m_banks[handle] = bank;
			return handle;
		}

		int Put(
			const std::string& path,
			FMOD::Studio::EventDescription* event)
		{
			int handle = MakeHandle(H_EVENT);
			PutLoaded(path, handle);
			m_events[handle] = event;
			return handle;
		}

		int Put(
			const std::string& path,
			FMOD::Studio::EventInstance* instance)
		{
			int handle = MakeHandle(H_INSTANCE);
			PutLoaded(path, handle);
			m_instances[handle] = instance;
			return handle;
		}

		int Put(
			const std::string& path,
			FMOD::Studio::Bus* bus)
		{
			int handle = MakeHandle(H_BUS);
			PutLoaded(path, handle);
			m_buses[handle] = bus;
			return handle;
		}

		int Put(
			const std::string& path,
			FMOD::Studio::VCA* vca)
		{
			int handle = MakeHandle(H_VCA);
			PutLoaded(path, handle);
			m_vcas[handle] = vca;
			return handle;
		}

		template<
			typename _t,
			typename _f1,
			typename _f2>
		int LoadFromBank(
			FMOD::Studio::Bank* bank,
			_f1&& getCount,
			_f2&& getItems)
		{
			int count;
			CHECK_ERROR(
				getCount(bank, count),
				ENGINE_FAILED_LOAD_BANK
			);

			if (count > 0)
			{
				_t** items = new _t *[count];

				CHECK_ERROR(
					getItems(bank, items, count),
					ENGINE_FAILED_LOAD_BANK
				);

				for (int i = 0; i < count; i++)
				{
					std::string name(1024, '\0');
					int size;

					CHECK_ERROR(
						items[0]->getPath(name.data(), 1024, &size),
						ENGINE_FAILED_LOAD_BANK
					);

					name.resize(size - 1);

					Put(name, items[i]);
				}

				delete[] items;
			}

			return ENGINE_OK;
		}

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

			if (result != FMOD_OK)
			{
				LOG_ERROR << "Fmod error code: " << FMODtranslation.at(result) << "(" << result << ").";
				log(code);
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
