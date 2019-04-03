#pragma once

#include "iw/input/IwInput.h"
#include "iw/events/functional/callback.h"

namespace IwInput {
#ifdef IW_PLATFORM_WINDOWS
	struct OsEvent {
		int WindowId;
		int Message;
		unsigned int  WParam;
		unsigned long LParam;
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
			, Name(INPUT_NONE)
			, State(0.0f)
			, WindowId(windowId)
		{}
	};

	using InputCallback       = iwevents::callback<InputEvent&>;
	using MouseWheelCallback  = iwevents::callback<float>;
	using MouseMovedCallback  = iwevents::callback<float, float, float, float>;
	using MouseButtonCallback = iwevents::callback<InputName, bool>;
	using KeyCallback         = iwevents::callback<InputName, bool>;
}
