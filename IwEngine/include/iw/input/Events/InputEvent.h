#pragma once

#include "iw/input/IwInput.h"
#include "iw/input/InputState.h"
#include "iw/events/functional/callback.h"

namespace IwInput {
#ifdef IW_PLATFORM_WINDOWS
	struct OsEvent {
		int    WindowId;
		int    Message;
		size_t WParam;
		size_t LParam;
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
	using MouseWheelCallback  = iwevents::callback<InputState, float>;
	using MouseMovedCallback  = iwevents::callback<InputState, float, float, float, float>;
	using MouseButtonCallback = iwevents::callback<InputState, InputName, bool>;
	using KeyCallback         = iwevents::callback<InputState, InputName, bool>;
	using KeyTypedCallback    = iwevents::callback<InputState, InputName, char>;
}
