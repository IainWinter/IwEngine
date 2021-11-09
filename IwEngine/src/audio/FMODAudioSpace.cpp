#include "iw/audio/FMODAudioSpace.h"

namespace iw {
namespace Audio {
	IAudioSpace* IAudioSpace::Create()
	{
		return new FMODAudioSpace();
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
