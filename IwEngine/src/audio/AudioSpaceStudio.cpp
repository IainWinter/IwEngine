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

	int AudioSpaceStudio::Initialize() {
		FMOD_RESULT error;

		error = FMOD::Studio::System::create(&m_system);
		CHECK_ERROR(error, "Audio space did not get created!", 1);

		error = m_system->initialize(GetChannels(), FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
		CHECK_ERROR(error, "Audio space did not get initliaze!", 2);

		return 0;
	}

	void AudioSpaceStudio::Update() {
		m_system->update();
	}

	void AudioSpaceStudio::LoadMasterBank(
		std::string name)
	{
		std::string path = GetRootDir() + name;

		auto itr = m_banks.find(name);
		if (itr != m_banks.end()) {
			itr->second->unload();
			delete itr->second; // ?
			itr->second = nullptr;
		}

		FMOD_RESULT error;
		error = m_system->loadBankFile(path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_banks[name]);
		CHECK_ERROR(error, "Master bank did not get loaded!");
	}

	void AudioSpaceStudio::CreateEvent(
		std::string name)
	{
		std::string path = "event:/" + name;

		auto itr = m_events.find(name);
		if (itr != m_events.end()) {
			LOG_INFO << "Event already created!";
			return;
		}

		FMOD_RESULT error;
		error = m_system->getEvent(path.c_str(), &m_events[name]);
		CHECK_ERROR(error, "Event could not be found!");
	}

	int AudioSpaceStudio::CreateInstance(
		std::string name,
		bool realeaseAtEnd)
	{
		std::string path = "event:/" + name;

		auto itr = m_events.find(name);
		if (itr == m_events.end()) {
			LOG_INFO << "Cannot find event!";
			return -1;
		}

		FMOD::Studio::EventInstance*& instance = m_instances.emplace_back(nullptr);

		FMOD_RESULT error;
		error = itr->second->createInstance(&instance);
		CHECK_ERROR(error, "Event could not be instanced!", -1);

		instance->start();

		if (realeaseAtEnd) {
			instance->setUserData(this);
			instance->setCallback(__dontlookatme__RemoveInstanceFromList, FMOD_STUDIO_EVENT_CALLBACK_STOPPED);
		}

		return m_instances.size();
	}

	void AudioSpaceStudio::StartInstance(
		int index)
	{
		m_instances.at(index - 1)->start();
	}

	void AudioSpaceStudio::StopInstance(
		int index)
	{
		m_instances.at(index - 1)->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}

	void AudioSpaceStudio::RemoveInstance(
		int index)
	{
		m_instances.at(index)->release();
		m_instances.erase(m_instances.begin() + index);
	}

	void AudioSpaceStudio::RemoveInstance(
		FMOD::Studio::EventInstance* instance)
	{
		auto itr = std::find(m_instances.begin(), m_instances.end(), instance);
		if (itr == m_instances.end()) {
			LOG_WARNING << "Cannot find instance!";
			return;
		}

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
