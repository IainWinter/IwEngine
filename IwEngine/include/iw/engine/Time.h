#pragma once

#include "Core.h"
#include <chrono>

namespace IW {
namespace Time {
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
	using Duration  = std::chrono::high_resolution_clock::duration;

	IWENGINE_API void SetFixedTime(
		float t);

	IWENGINE_API void   UpdateTime();
	IWENGINE_API size_t Ticks();      //Probaly should use unsigned long long
	IWENGINE_API float  TotalTime();
	IWENGINE_API float  DeltaTime(); 
	IWENGINE_API float  DeltaTimeNow();
	IWENGINE_API float  FixedTime();
}

	using namespace Time;
}
