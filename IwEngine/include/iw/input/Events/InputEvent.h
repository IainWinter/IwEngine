#pragma once

#include "iw/input/IwInput.h"

namespace IwInput {
	struct OsEvent {
		int   WindowId;
		int   Message;
		short WParam;
		int   LParam;
	};

	struct InputEvent {
		DeviceType DeviceType;
		InputName Name;
		union {
			bool StateBool;
			int StateInt;
			float StateFloat;
			double StateDouble;
		};
	};
}
