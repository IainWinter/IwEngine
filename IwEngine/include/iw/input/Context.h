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

		std::vector<iw::ref<Device>> Devices;
		iw::callback<> Translator; // well this could also be somewhere with an event bus or 

	public:
		std::string Name;
		float Width;
		float Height;

		InputState State;

		Context() = default;

		IWINPUT_API
		Context(
			std::string name,
			float width,
			float height);

		IWINPUT_API
		void AddDevice(
			const iw::ref<Device>& device);

		IWINPUT_API
		void RemoveDevice(
			const iw::ref<Device>& device);
	};
}
}
