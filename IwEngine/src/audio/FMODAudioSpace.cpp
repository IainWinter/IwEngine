#include "iw/audio/FMODAudioSpace.h"

namespace iw {
namespace Audio {
	IAudioSpace* IAudioSpace::Create()
	{
		return new FMODAudioSpace();
	}

	int FMODAudioSpace::Initialize()
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

	int FMODAudioSpace::Update()
	{
		CHECK_ERROR(
			m_system->update(),
			ENGINE_FAILED_UPDATE
		);

		return ENGINE_OK;
	}

	int FMODAudioSpace::Destroy()
	{
		// find how to destroy

		return ENGINE_OK;
	}

	int FMODAudioSpace::Load(
		const std::string& path)
	{
		// this could be loaded a bank or an event or a sound
		// depending on what path is, could add 

		std::string p = RootDirectory + path;

		if (IAudioSpace::IsLoaded(path))
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

	int FMODAudioSpace::Play(
		const std::string& path)
	{
		if (path.find("event:/") == 0)
		{
			int eventHandle = GetHandle(path);

			if (eventHandle <= 0)
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
				FMOD_STUDIO_EVENTINSTANCE* einstance,
				void* parameters)
			{
				// wtf is this api
				FMOD::Studio::EventInstance* instance = 
					(FMOD::Studio::EventInstance*)einstance;
				
				InstanceUserData* data;
				instance->getUserData((void**)&data);
				auto [inst, me] = *data;
				me->m_instances.erase(inst);
				return FMOD_OK;
			};

			// this overwrites the Loaded list everytime one spawns,
			// that might be ok? like GetHandle(path) gives you the last spawned
			// but all the handles are still valid
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

	int FMODAudioSpace::Set(
		int handle, 
		const std::string& parameter, 
		float value)
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

	int FMODAudioSpace::Get(
		int handle, 
		const std::string& parameter, 
		float& value)
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

	int FMODAudioSpace::Start(
		int handle)
	{
		CHECK_HANDLE(handle);

		if (GetType(handle) == H_INSTANCE)
		{
			m_instances[handle]->start();
			return ENGINE_OK;
		}

		return log(ENGINE_FAILED_INVALID_HANDLE); // dif flow than other with !!bottom should return ENGINE_OK!!
	}

	int FMODAudioSpace::Stop(
		int handle)
	{
		CHECK_HANDLE(handle);

		if (GetType(handle) == H_INSTANCE)
		{
			m_instances[handle]->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
			return ENGINE_OK;
		}

		return log(ENGINE_FAILED_INVALID_HANDLE);
	}

	int FMODAudioSpace::Free(
		int handle)
	{
		CHECK_HANDLE(handle);

		if (GetType(handle) == H_INSTANCE)
		{
			m_instances[handle]->release();
			return ENGINE_OK;
		}

		return log(ENGINE_FAILED_INVALID_HANDLE);
	}

	int FMODAudioSpace::SetVolume(
		int handle,
		float volume)
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

	int FMODAudioSpace::GetVolume(
		int handle,
		float& volume)
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

	bool FMODAudioSpace::IsLoaded(
		int handle) const
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

	bool FMODAudioSpace::CheckError(
		int result,
		AudioErrorCode code) const
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
}
}

//#include "iw/log/logger.h"
//#include "fmod/fmod_studio.hpp"
//
//#pragma once
//
//#ifdef IW_DEBUG
//#define CHECK_ERROR(stmt, msg, n) stmt
//#else
//#define CHECK_ERROR(stmt, msg, n) if(CheckError(stmt, msg)) return n;
//#endif
//
//
//namespace iw {
//namespace Audio {
//	AudioSpaceStudio::AudioSpaceStudio(
//		std::string rootDir,
//		int channels,
//		float volume)
//		: AudioSpace(rootDir, channels, volume)
//		, m_system(nullptr)
//	{}
//
//	AudioSpaceStudio::~AudioSpaceStudio() {
//		for (auto itr : m_banks) {
//			itr.second->unload();
//		}
//
//		for (auto itr : m_events) {
//			itr.second->releaseAllInstances();
//		}
//
//		m_system->release();
//	}
//
//	void AudioSpaceStudio::CreateEvent(
//		std::string name)
//	{
//		std::string path = "event:/" + name;
//
//		LOG_INFO << "Loading event from " << path;
//
//		auto itr = m_events.find(name);
//#ifdef IW_DEBUG
//		if (itr != m_events.end()) {
//			LOG_WARNING << "Event " << name << " already created!";
//			return;
//		}
//#endif
//		CheckError(
//			m_system->getEvent(path.c_str(), &m_events[name]),
//			"Event " + name + " could not be found!"
//		);
//	}
//
//	int AudioSpaceStudio::CreateInstance(
//		std::string name,
//		bool realeaseAtEnd)
//	{
//		std::string path = "event:/" + name;
//		
//		LOG_INFO << "Creating audio instance of " << path;
//
//		auto itr = m_events.find(name);
//#ifdef IW_DEBUG
//		if (itr == m_events.end()) {
//			LOG_WARNING << "Cannot find event " << name << "!";
//			return -1;
//		}
//#endif
//		FMOD::Studio::EventInstance*& instance = m_instances.emplace_back(nullptr);
//
//		CheckError(
//			itr->second->createInstance(&instance),
//			"Event " + name + " could not be instanced!"/*, -1*/
//		);
//
//		instance->start();
//		instance->getChannelGroup();
//
//		if (realeaseAtEnd) {
//			instance->setUserData(this);
//			instance->setCallback((FMOD_STUDIO_EVENT_CALLBACK)__dontlookatme__RemoveInstanceFromList, FMOD_STUDIO_EVENT_CALLBACK_STOPPED);
//		}
//
//		return (int)m_instances.size() - 1;
//	}
//
//	void AudioSpaceStudio::SetInstanceParameter(
//		int instance,
//		std::string name,
//		float value)
//	{
//		if (0 > instance || instance >= m_instances.size()) {
//			LOG_WARNING << "Cannot start instance " << instance << "; index out of range!";
//			return;
//		}
//
//		LOG_INFO << "Set audio instance parameter " << name << " to " << value;
//
//		m_instances.at(instance)->setParameterByName(name.c_str(), value);
//	}
//
//	void AudioSpaceStudio::StartInstance(
//		int index) // make unsigned
//	{
//		LOG_INFO << "Starting audio instance " << index;
//
//		if (0 > index || index >= m_instances.size()) {
//			LOG_WARNING << "Cannot start instance " << index << "; index out of range!";
//			return;
//		}
//
//		m_instances.at(index)->start();
//	}
//
//	void AudioSpaceStudio::StopInstance(
//		int index)
//	{
//		LOG_INFO << "Stopping audio instance " << index;
//
//#ifdef IW_DEBUG
//		if (0 > index || index >= m_instances.size()) {
//			LOG_WARNING << "Cannot stop instance " << index << "; index out of range!";
//			return;
//		}
//#endif
//		m_instances.at(index)->stop(FMOD_STUDIO_STOP_IMMEDIATE);
//	}
//
//	void AudioSpaceStudio::SetVolume(
//		float volume)
//	{
//		FMOD::Studio::ID masterBusID;
//		m_system->lookupID("/", &masterBusID);
//
//		m_system->getAdvancedSettings(&masterBusID, FMOD_STUDIO_LOAD_PROHIBITED, &masterBusID);
//	}
//
//	void AudioSpaceStudio::RemoveInstance( // removeing breaks other index
//		int index)
//	{
//		LOG_INFO << "Removing audio instance " << index;
//
//#ifdef IW_DEBUG
//		if (0 > index || index >= m_instances.size()) {
//			LOG_WARNING << "Cannot remove instance " << index << "; index out of range!";
//			return;
//		}
//#endif
//
//		m_instances.at(index)->release();
//
//		m_instances.at(index) = nullptr; // remove only if at the end to keep instances correct
//		while (m_instances.back() == nullptr) {
//			m_instances.pop_back();
//		}
//
//		//m_instances.erase(m_instances.begin() + index);
//	}
//
//	void AudioSpaceStudio::RemoveInstance(
//		FMOD::Studio::EventInstance* instance)
//	{
//		auto itr = std::find(m_instances.begin(), m_instances.end(), instance); // isnt this redundant cus they ptrs???
//		RemoveInstance(std::distance(m_instances.begin(), itr));
//	}
//
//	FMOD_RESULT __dontlookatme__RemoveInstanceFromList(
//		FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
//		FMOD_STUDIO_EVENTINSTANCE* event,
//		void* parameters)
//	{
//		LOG_INFO << "Freeing audio instance " << (void*)event;
//
//		FMOD::Studio::EventInstance* instance = ((FMOD::Studio::EventInstance*)event);
//
//		AudioSpaceStudio* me = nullptr;
//		instance->getUserData((void**)&me);
//		
//		if (me) {
//			me->RemoveInstance(instance);
//		}
//
//		return FMOD_OK;
//	}
//}
//}
