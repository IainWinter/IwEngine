#include "iw/audio/AudioSpaceStudio.h"
#include "iw/log/logger.h"
#include "CHECK_ERROR.h"

namespace iw {
namespace Audio {
	AudioSpaceStudio::AudioSpaceStudio(
		std::string rootDir,
		int channels,
		float volume)
		: AudioSpace(rootDir, channels, volume)
		, m_system(nullptr)
	{}

	AudioSpaceStudio::~AudioSpaceStudio() {
		for (auto itr : m_banks) {
			itr.second->unload();
		}

		for (auto itr : m_events) {
			itr.second->releaseAllInstances();
		}

		m_system->release();
	}

	int AudioSpaceStudio::Initialize() {
		CHECK_ERROR(
			FMOD::Studio::System::create(&m_system),
			"Audio space did not get created!", 1
		);

		CHECK_ERROR(
			m_system->initialize(GetChannels(), FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr),
			"Audio space did not get initliaze!", 2
		);

		return 0;
	}

	void AudioSpaceStudio::Update() {
		CHECK_ERROR(
			m_system->update(),
			"Audio space could failed to update!"
		);
	}

	void AudioSpaceStudio::LoadMasterBank(
		std::string name)
	{
		std::string path = GetRootDir() + name;

		auto itr = m_banks.find(name);
		if (itr != m_banks.end()) {
			itr->second->unload();
		}

		CHECK_ERROR(
			m_system->loadBankFile(path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_banks[name]),
			"Master bank did not get loaded!"
		);
	}

	void AudioSpaceStudio::CreateEvent(
		std::string name)
	{
		std::string path = "event:/" + name;

		auto itr = m_events.find(name);
#ifdef IW_DEBUG
		if (itr != m_events.end()) {
			LOG_WARNING << "Event " << name << " already created!";
			return;
		}
#endif
		CHECK_ERROR(
			m_system->getEvent(path.c_str(), &m_events[name]),
			"Event " + name + " could not be found!"
		);
	}

	int AudioSpaceStudio::CreateInstance(
		std::string name,
		bool realeaseAtEnd)
	{
		std::string path = "event:/" + name;

		auto itr = m_events.find(name);
#ifdef IW_DEBUG
		if (itr == m_events.end()) {
			LOG_WARNING << "Cannot find event " << name << "!";
			return -1;
		}
#endif
		FMOD::Studio::EventInstance*& instance = m_instances.emplace_back(nullptr);

		CHECK_ERROR(
			itr->second->createInstance(&instance),
			"Event " + name + " could not be instanced!", -1
		);

		instance->start();

		if (realeaseAtEnd) {
			instance->setUserData(this);
			instance->setCallback((FMOD_STUDIO_EVENT_CALLBACK)__dontlookatme__RemoveInstanceFromList, FMOD_STUDIO_EVENT_CALLBACK_STOPPED);
		}

		return m_instances.size() - 1;
	}

	void AudioSpaceStudio::StartInstance(
		int index)
	{
#ifdef IW_DEBUG
		if (0 > index || index >= m_instances.size()) {
			LOG_WARNING << "Cannot start instance " << index << "; index out of range!";
			return;
		}
#endif
		m_instances.at(index)->start();
	}

	void AudioSpaceStudio::StopInstance(
		int index)
	{
#ifdef IW_DEBUG
		if (0 > index || index >= m_instances.size()) {
			LOG_WARNING << "Cannot stop instance " << index << "; index out of range!";
			return;
		}
#endif
		m_instances.at(index)->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}

	void AudioSpaceStudio::RemoveInstance(
		int index)
	{
#ifdef IW_DEBUG
		if (0 > index || index >= m_instances.size()) {
			LOG_WARNING << "Cannot remove instance " << index << "; index out of range!";
			return;
		}
#endif
		m_instances.at(index)->release();
		m_instances.erase(m_instances.begin() + index);
	}

	void AudioSpaceStudio::RemoveInstance(
		FMOD::Studio::EventInstance* instance)
	{
		auto itr = std::find(m_instances.begin(), m_instances.end(), instance);
#ifdef IW_DEBUG
		if (itr == m_instances.end()) {
			LOG_WARNING << "Cannot remove instance " << instance << "; cannot find instance!";
			return;
		}
#endif
		instance->release();
		m_instances.erase(itr);
	}

	FMOD_RESULT __dontlookatme__RemoveInstanceFromList(
		FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
		FMOD_STUDIO_EVENTINSTANCE* event,
		void* parameters)
	{
		FMOD::Studio::EventInstance* instance = ((FMOD::Studio::EventInstance*)event);

		AudioSpaceStudio* me = nullptr;
		instance->getUserData((void**)&me);

		if (me) {
			me->RemoveInstance(instance);
		}

		return FMOD_OK;
	}
}
}
