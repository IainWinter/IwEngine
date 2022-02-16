#pragma once

#include "Context.h"
#include "Devices/Mouse.h"
#include "Devices/Keyboard.h"
#include "iw/events/eventbus.h"
#include <vector>

namespace iw {
namespace Input {
	class InputManager {
	private:
		std::vector<ref<Context>> m_contexts;
		std::vector<ref<Device>>  m_devices;
		ref<eventbus> m_bus;
		ref<Context> m_active;

	public:
		IWINPUT_API
		InputManager(
			const ref<eventbus>& bus);

		IWINPUT_API
		void HandleEvent(
			event& e);

		IWINPUT_API
		ref<Context> CreateContext(
			const std::string& name,
			float width  = NO_WIDTH,
			float height = NO_HEIGHT);

		IWINPUT_API
		ref<Context> SetContext(
			const std::string& name);

		IWINPUT_API
		void SetContext(
			const ref<Context>& context);

		IWINPUT_API
		ref<Context> GetContext(
			const std::string& name = "");

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
