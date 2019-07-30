#pragma once

#include "WindowOptions.h"

namespace IwEngine {
	struct InitOptions {
		WindowOptions WindowOptions;
		void*         ImGuiContext; //ImGuiContext
	};
}
