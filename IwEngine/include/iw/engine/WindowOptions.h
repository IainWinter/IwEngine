#pragma once

#include "iw/engine/DisplayState.h"

namespace iw {
namespace Engine {
	struct WindowOptions {
		unsigned int Width;
		unsigned int Height;
		bool         Cursor;
		DisplayState State;
	};
}

	using namespace Engine;
}
