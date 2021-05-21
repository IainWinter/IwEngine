#pragma once

#include "iw/engine/WindowOptions.h"
#include <string>

namespace iw {
namespace Engine {
	struct InitOptions {
		WindowOptions WindowOptions;
		void*         ImGuiContext; //ImGuiContext
		std::string   AssetRootPath = "C:/dev/IwEngine/_assets/";
	};
}

	using namespace Engine;
}
