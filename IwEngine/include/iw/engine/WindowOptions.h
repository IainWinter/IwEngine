#pragma once

#include "DisplayState.h"

namespace IW {
inline namespace Engine {
	struct WindowOptions {
		unsigned int Width;
		unsigned int Height;
		bool         Cursor;
		DisplayState State;
	};
}
}
