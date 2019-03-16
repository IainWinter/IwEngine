#pragma once

#include "iw/input/IwInput.h"
#include "iw/events/functional/callback.h"

namespace IwInput {
#ifdef IW_PLATFORM_WINDOWS
	struct OsEvent {
		int   WindowId;
		int   Message;
		short WParam;
		int   LParam;
	};
#endif

	struct InputEvent {
		DeviceType Device;
		InputName  Name;
		float      State;
		int        WindowId;

		InputEvent(
			DeviceType deviceType,
			int windowId)
			: Device(deviceType)
			, WindowId(windowId)
		{}
	};

	using InputCallback       = iwevents::callback<InputEvent&>;
	using MouseWheelCallback  = iwevents::callback<float>;
	using MouseMovedCallback  = iwevents::callback<float, float, float, float>;
	using MouseButtonCallback = iwevents::callback<InputName, bool>;
	using KeyCallback         = iwevents::callback<InputName, bool>;
}
