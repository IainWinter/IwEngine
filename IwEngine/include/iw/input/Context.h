#pragma once

#include "InputState.h"
#include "Devices/Device.h"
#include "iw/util/memory/ref.h"

namespace iw {
namespace Input {
	class InputManager;

	struct Context {
	private:
		friend class InputManager;

		std::vector<ref<Device>> m_devices;
		std::unordered_map<InputName, std::string> m_commands; // well this could also be somewhere with an event bus or 

	public:
		std::string Name;
		float Width;
		float Height;

		InputState State;
		InputName LastInput; // last named input to be processed

		IWINPUT_API
		Context(
			std::string name,
			float width,
			float height);

		IWINPUT_API
		void AddDevice(
			ref<Device>& device);

		IWINPUT_API
		void RemoveDevice(
			ref<Device>& device);

		IWINPUT_API
		void MapButton(
			InputName input,
			std::string command);
	};
}

	using namespace Input;
}
