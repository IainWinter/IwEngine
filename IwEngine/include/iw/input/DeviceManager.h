#pragma once 

#include "IwInput.h"
#include "Devices/Device.h"
#include "Devices/Mouse.h"
#include "Devices/Keyboard.h"
#include "Events/InputEvent.h"
#include "iw/log/logger.h"
#include <vector>

namespace IW {
inline namespace Input {
	class IWINPUT_API DeviceManager {
	private:
		struct{
			std::vector<Device*> m_devices;
		};

	public:
		void HandleEvent(
			OsEvent event);

		template<
			typename _Device_T>
		void CreateDevice(
			InputCallback callback)
		{
			LOG_WARNING << "Attempted to create invalid device!";
		}

		template<>
		void CreateDevice<Mouse>(
			InputCallback callback);

		template<>
		void CreateDevice<Keyboard>(
			InputCallback callback);

#ifdef IW_PLATFORM_WINDOWS
		template<>
		void CreateDevice<RawMouse>(
			InputCallback callback);

		template<>
		void CreateDevice<RawKeyboard>(
			InputCallback callback);
#endif
	};
}
}
