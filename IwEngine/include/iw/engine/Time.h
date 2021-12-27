#pragma once

#include "iw/engine/Core.h"
#include <chrono>

namespace iw {
namespace Time {
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
	using Duration  = std::chrono::high_resolution_clock::duration;

	IWENGINE_API void SetDeltaTime(float t);
	IWENGINE_API void SetFixedTime(float t);
	IWENGINE_API void SetTimeScale(float t);

	IWENGINE_API void UpdateTime();

	IWENGINE_API size_t Ticks();      //Probaly should use unsigned long long
	IWENGINE_API float  TotalTime();
	IWENGINE_API float  RawTotalTime();

	IWENGINE_API float RawDeltaTime();
	IWENGINE_API float DeltaTime();
	IWENGINE_API float SmoothDeltaTime();
	IWENGINE_API float DeltaTimeNow();

	IWENGINE_API float RawFixedTime();
	IWENGINE_API float FixedTime();

	IWENGINE_API float TimeScale();

	IWENGINE_API std::pair<float*, size_t> Times();
}

	using namespace Time;
}
