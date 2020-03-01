#pragma once

#include "AudioSpace.h"
#include "fmod/fmod_studio.hpp"
#include <string>
#include <vector>
#include <unordered_map>

#undef CreateEvent

namespace iw {
namespace Audio {
	FMOD_RESULT __dontlookatme__RemoveInstanceFromList(
		FMOD_STUDIO_EVENT_CALLBACK_TYPE type,
		FMOD_STUDIO_EVENTINSTANCE* event,
		void* parameters);

	class AudioSpaceStudio
		: public AudioSpace
	{
	private:
		FMOD::Studio::System* m_system;

		std::unordered_map<std::string, FMOD::Studio::Bank*> m_banks;
		std::unordered_map<std::string, FMOD::Studio::EventDescription*> m_events;
		std::vector<FMOD::Studio::EventInstance*> m_instances;

		friend FMOD_RESULT iw::Audio::__dontlookatme__RemoveInstanceFromList(
			FMOD_STUDIO_EVENT_CALLBACK_TYPE,
			FMOD_STUDIO_EVENTINSTANCE*,
			void*);

	public:
		IWAUDIO_API
		AudioSpaceStudio(
			std::string rootDir = "",
			int channels = 32,
			float volume = 1.0f);

		IWAUDIO_API
		int Initialize() override;

		IWAUDIO_API
		void Update() override;

		IWAUDIO_API
		void LoadMasterBank(
			std::string name);

		IWAUDIO_API
		void CreateEvent(
			std::string name);

		IWAUDIO_API
		int CreateInstance(
			std::string name,
			bool realeaseAtEnd = true);

		IWAUDIO_API
		void RemoveInstance(
			int index);

		IWAUDIO_API
		void StartInstance(
			int index);

		IWAUDIO_API
		void StopInstance(
			int index);
	private:
		IWAUDIO_API
		void RemoveInstance(
			FMOD::Studio::EventInstance* instance);
	};
}

	using namespace Audio;
}
