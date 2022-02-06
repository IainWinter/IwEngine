#pragma once

#include "iwinput.h"
#include <string>

namespace iw {
namespace Input {
	struct DeviceInput {
		DeviceType Device;
		InputName  Name;
		InputName  Name2;
		float      State;
		float      State2;

		DeviceInput(
			DeviceType deviceType
		)
			: Device (deviceType)
			, Name   (InputName::INPUT_NONE)
			, Name2  (InputName::INPUT_NONE)
			, State  (0.f)
			, State2 (0.f)
		{}
	};
}

	using namespace Input;
}
