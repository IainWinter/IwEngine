#pragma once

#include "iw/engine/DisplayState.h"

namespace iw {
namespace Engine {
	struct WindowOptions {
		unsigned int Width  = 800*16/9.f;
		unsigned int Height = 800;
		bool         Cursor = true;
		DisplayState State = DisplayState::NORMAL;
	};
}

	using namespace Engine;
}
