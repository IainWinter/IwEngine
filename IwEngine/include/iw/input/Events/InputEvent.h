#pragma once

#include "iw/input/IwInput.h"
#include "iw/input/InputState.h"
#include "iw/events/callback.h"

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

	using InputCallback       = iw::callback<InputEvent&>;
	using MouseWheelCallback  = iw::callback<InputState, float>;
	using MouseMovedCallback  = iw::callback<InputState, float, float, float, float>;
	using MouseButtonCallback = iw::callback<InputState, InputName, bool>;
	using KeyCallback         = iw::callback<InputState, InputName, bool>;
	using KeyTypedCallback    = iw::callback<InputState, InputName, char>;
}
