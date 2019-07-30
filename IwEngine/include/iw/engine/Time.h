#pragma once

#include "Core.h"
#include <chrono>

namespace IwEngine {
	namespace Time {
		using Time     = std::chrono::time_point<std::chrono::steady_clock>;
		using Duration = std::chrono::high_resolution_clock::duration;

		IWENGINE_API void SetFixedTime(
			float t);

		IWENGINE_API void  Update();
		IWENGINE_API int   Ticks();
		IWENGINE_API float TotalTime();
		IWENGINE_API float DeltaTime(); //Returns time in seconds as a double
		IWENGINE_API float FixedTime();
	}
}
