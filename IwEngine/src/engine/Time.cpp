#include "iw/engine/Time.h"
#include "iw/log/logger.h"

namespace iw {
namespace Time {
	static TimePoint now       = std::chrono::high_resolution_clock::now();
	static Duration  deltaTime = Duration::zero();
	static size_t    ticks     = 0;
	static float     time      = 0.0f;
	static float     fixedTime = 0.2f;
	static float     timeScale = 1.0f;

	void SetFixedTime(
		float duration) 
	{
		fixedTime = duration;
		LOG_DEBUG << "Set fixed timeset to " << duration << " seconds";
	}

	void SetTimeScale(
		float duration)
	{
		timeScale = duration;
		LOG_DEBUG << "Set time scale to " << duration << " seconds";
	}

	void UpdateTime() {
		ticks++;
		time += DeltaTime();
		deltaTime = std::chrono::high_resolution_clock::now() - now;
		now = std::chrono::high_resolution_clock::now();
	}

	size_t Ticks() {
		return ticks;
	}

	float TotalTime() {
		return time;
	}

	float DeltaTime() {
		return deltaTime.count() / 1000000000.0f;
	}

	float DeltaTimeScaled() {
		return DeltaTime() * TimeScale();
	}

	// Idk why this is like 1000 times slower
	float DeltaTimeNow() {
		return (std::chrono::high_resolution_clock::now() - now).count() / 1000000000.0f;
	}

	float RawFixedTime() {
		return fixedTime;
	}

	float FixedTime() {
		return RawFixedTime() * TimeScale();
	}

	float TimeScale() {
		return timeScale;
	}
}
}
