#pragma once

#include "DisplayState.h"

namespace IwEngine {
	struct WindowOptions {
		unsigned int Width;
		unsigned int Height;
		bool         Cursor;
		DisplayState State;
	};
}
