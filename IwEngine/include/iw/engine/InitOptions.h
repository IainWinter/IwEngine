#pragma once

#include "iw/engine/WindowOptions.h"
#include <string>

namespace iw {
namespace Engine {
	struct InitOptions {
		WindowOptions WindowOptions;
		void*         ImGuiContext; //ImGuiContext
		std::string   AssetRootPath; // default if "" gets set in Application.cpp to IW_ASSET_ROOT_PATH set from premake5
	};
}

	using namespace Engine;
}
