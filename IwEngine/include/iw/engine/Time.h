#pragma once

#include "Core.h"
#include <chrono>

namespace IwEngine {
	namespace Time {
		using Time     = std::chrono::time_point<std::chrono::steady_clock>;
		using Duration = std::chrono::high_resolution_clock::duration;

		IWENGINE_API Time   Update();
		IWENGINE_API int    Ticks();
		IWENGINE_API double DeltaTime(); //Returns time in seconds as a double
	}
}
