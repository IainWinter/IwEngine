#pragma once

#include "iw/engine/DisplayState.h"

namespace iw {
namespace Engine {
	struct WindowOptions {
		unsigned int Width  = 800;
		unsigned int Height = 600;
		bool         Cursor = true;
		DisplayState State = DisplayState::NORMAL;
	};
}

	using namespace Engine;
}
