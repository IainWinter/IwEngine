#pragma once

#include "iwinput.h"
#include "InputState.h"
#include "Devices/Device.h"
#include "iw/events/event.h"
#include "iw/events/callback.h"
#include "iw/util/memory/smart_pointers.h"
#include <string>

namespace IW {
inline namespace Input {
	class InputManager;

	struct Context {
	private:
		friend class InputManager;

		std::vector<iw::ref<Device>> m_devices;
		std::unordered_map<InputName, std::string> m_commands; // well this could also be somewhere with an event bus or 

	public:
		std::string Name;
		float Width;
		float Height;

		InputState State;

		IWINPUT_API
		Context(
			std::string name,
			float width,
			float height);

		IWINPUT_API
		void AddDevice(
			iw::ref<Device>& device);

		IWINPUT_API
		void RemoveDevice(
			iw::ref<Device>& device);

		IWINPUT_API
		void MapButton(
			InputName input,
			std::string command);
	};
}
}
