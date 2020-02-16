#pragma once

#include "iwinput.h"
#include "Context.h"
#include "Devices/Device.h"
#include "Devices/Mouse.h"
#include "Devices/Keyboard.h"
#include "iw/events/eventbus.h"
#include "iw/util/memory/ref.h"
#include <vector>

namespace iw {
namespace Input {
	class InputManager {
	private:
		std::vector<ref<Context>> m_contexts;
		std::vector<ref<Device>>  m_devices;
		ref<Context> m_active;
		ref<eventbus> m_bus;

	public:
		IWINPUT_API
		InputManager(
			ref<eventbus>& bus);

		IWINPUT_API
		void HandleEvent(
			event& e);

		IWINPUT_API
		ref<Context> CreateContext(
			std::string name,
			float width  = NO_WIDTH,
			float height = NO_HEIGHT);

		template<
			typename D>
		ref<Device> CreateDevice() {
			LOG_WARNING << "Attempted to create invalid device!";
			assert(false);
			return nullptr;
		}

		template<>
		IWINPUT_API
		ref<Device> CreateDevice<Mouse>();

		template<>
		IWINPUT_API
		ref<Device> CreateDevice<Keyboard>();

		template<>
		IWINPUT_API
		ref<Device> CreateDevice<RawMouse>();

		template<>
		IWINPUT_API
		ref<Device> CreateDevice<RawKeyboard>();
	private:
		//bool TryAddDevice(
		//	ref<Device>& device);
	};
}

	using namespace Input;
}
