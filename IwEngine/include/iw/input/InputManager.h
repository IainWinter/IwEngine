#pragma once

#include "Context.h"
#include "Devices/Device.h"
#include "Devices/Mouse.h"
#include "Devices/Keyboard.h"
#include "iw/util/memory/smart_pointers.h"
#include "iw/events/eventbus.h"

namespace IW {
inline namespace Input {
	class IWINPUT_API InputManager {
	private:
		std::vector<iw::ref<Context>> m_contexts;
		std::vector<iw::ref<Device>>  m_devices;
		iw::ref<Context> m_active;

		iw::ref<iw::eventbus> m_bus;

	public:
		InputManager(
			iw::ref<iw::eventbus>& bus);

		void HandleEvent(
			iw::event& e);

		void CreateContext(
			std::string name,
			float width  = NO_WIDTH,
			float height = NO_HEIGHT);

		template<
			typename _D>
		iw::ref<Device>& CreateDevice(
			std::string name)
		{
			LOG_WARNING << "Attempted to create invalid device!";
			assert(false);
			return nullptr;
		}

		template<>
		iw::ref<Device>& CreateDevice<Mouse>(
			std::string name);

		template<>
		iw::ref<Device>& CreateDevice<Keyboard>(
			std::string name);

#ifdef IW_PLATFORM_WINDOWS
		template<>
		iw::ref<Device>& CreateDevice<RawMouse>(
			std::string name);

		template<>
		iw::ref<Device>& CreateDevice<RawKeyboard>(
			std::string name);
#endif
	private:
		bool TryAddDevice(
			std::string name,
			iw::ref<Device>& device);
	};
}
}
