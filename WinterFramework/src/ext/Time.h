#pragma once

#include <utility>
#include <stddef.h>

namespace Time
{
	void SetDeltaTime(float t);
	void SetFixedTime(float t);
	void SetTimeScale(float t);

	void UpdateTime();

	size_t Ticks();
	float TotalTime();
	float RawTotalTime();

	float RawDeltaTime();
	float DeltaTime();
	float SmoothDeltaTime();
	float DeltaTimeNow();

	float RawFixedTime();
	float FixedTime();

	float TimeScale();

	std::pair<float*, size_t> Times();
}