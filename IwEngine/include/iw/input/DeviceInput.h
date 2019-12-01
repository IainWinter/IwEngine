#pragma once

#include "iwinput.h"

namespace IW {
inline namespace  {
	struct DeviceInput {
		DeviceType Device;
		InputName  Name;
		InputName  Name2;
		float      State;
		float      State2;

		DeviceInput(
			DeviceType deviceType)
			: Device(deviceType)
			, Name (INPUT_NONE)
			, Name2(INPUT_NONE)
			, State (0.0f)
			, State2(0.0f)
		{}
	};
}
}