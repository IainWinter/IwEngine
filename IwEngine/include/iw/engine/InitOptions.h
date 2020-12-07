#pragma once

#include "iw/engine/WindowOptions.h"

namespace iw {
namespace Engine {
	struct InitOptions {
		WindowOptions WindowOptions;
		void*         ImGuiContext; //ImGuiContext
	};
}

	using namespace Engine;
}
