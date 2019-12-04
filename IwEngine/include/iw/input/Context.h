#pragma once

#include "InputState.h"
#include "Devices/Device.h"
#include "iw/events/event.h"
#include "iw/events/callback.h"
#include "iw/util/memory/smart_pointers.h"
#include <string>

namespace IW {
inline namespace Input {
	struct Context {
		std::string Name;
		float Width;
		float Height;

		InputState State;
		iw::callback<> Translator; // well this could also be somewhere with an event bus or 

		std::vector<iw::ref<Device>> Devices;

		Context() = default;

		Context(
			std::string name,
			float width,
			float height);

		void AddDevice(
			const iw::ref<Device>& device);

		void RemoveDevice(
			const iw::ref<Device>& device);
	};
}
}
