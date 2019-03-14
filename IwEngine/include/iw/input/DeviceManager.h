#pragma once 

#include "IwInput.h"
#include "Devices/Device.h"
#include "Events/InputEvent.h"
#include "iw/log/logger.h"
#include <vector>

namespace IwInput {
	class IWINPUT_API DeviceManager {
	private:
		struct{
			std::vector<IDevice*> m_devices;
		};

	public:
		template<
			typename _Device_T>
		void CreateDevice() {
			LOG_WARNING << "Attempted to create invalid device.";
		}

		void HandleEvent(
			OsEvent event)
		{
			for (IDevice* device : m_devices) {
				device->HandleEvent(event);
			}
		}
	};
}
