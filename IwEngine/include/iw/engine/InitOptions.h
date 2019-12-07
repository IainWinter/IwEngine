#pragma once

#include "WindowOptions.h"

namespace IW {
namespace Engine {
	struct InitOptions {
		WindowOptions WindowOptions;
		void*         ImGuiContext; //ImGuiContext
	};
}

	using namespace Engine;
}
