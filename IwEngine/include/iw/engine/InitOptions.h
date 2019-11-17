#pragma once

#include "WindowOptions.h"

namespace IW {
inline namespace Engine {
	struct InitOptions {
		WindowOptions WindowOptions;
		void*         ImGuiContext; //ImGuiContext
	};
}
}
