#pragma once

#include "iwinput.h"
#include "Context.h"
#include "Devices/Device.h"
#include "Devices/Mouse.h"
#include "Devices/Keyboard.h"
#include "iw/util/memory/ref.h"
#include "iw/events/eventbus.h"
#include <vector>

namespace iw {
namespace Input {
	class InputManager {
	private:
		std::vector<iw::ref<Context>> m_contexts;
		std::vector<iw::ref<Device>>  m_devices;
		iw::ref<Context> m_active;
		iw::ref<iw::eventbus> m_bus;

	public:
		IWINPUT_API
		InputManager(
			iw::ref<iw::eventbus>& bus);

		IWINPUT_API
		void HandleEvent(
			iw::event& e);

		IWINPUT_API
		iw::ref<Context> CreateContext(
			std::string name,
			float width  = NO_WIDTH,
			float height = NO_HEIGHT);

		template<
			typename D>
		iw::ref<Device> CreateDevice() {
			LOG_WARNING << "Attempted to create invalid device!";
			assert(false);
			return nullptr;
		}

		template<>
		IWINPUT_API
		iw::ref<Device> CreateDevice<Mouse>();

		template<>
		IWINPUT_API
		iw::ref<Device> CreateDevice<Keyboard>();

		template<>
		IWINPUT_API
		iw::ref<Device> CreateDevice<RawMouse>();

		template<>
		IWINPUT_API
		iw::ref<Device> CreateDevice<RawKeyboard>();
	private:
		//bool TryAddDevice(
		//	iw::ref<Device>& device);
	};
}

	using namespace Input;
}
