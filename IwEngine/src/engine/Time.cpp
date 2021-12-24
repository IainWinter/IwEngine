#include "iw/engine/Time.h"
#include "iw/log/logger.h"
#include <vector>

namespace iw {
namespace Time {
	static TimePoint now       = std::chrono::high_resolution_clock::now();
	static Duration  deltaTime = Duration::zero();
	static size_t    ticks     = 0;
	static float     time      = 0.0f;
	static float     fixedTime = 0.2f;
	static float     timeScale = 1.0f;

	static std::vector<float> pastDt;
	float smoothDeltaTime;

	void SetDeltaTime(
		float dt)
	{
		deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
				std::chrono::duration<float, std::ratio<1, 1>>(dt));
	}

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

		pastDt.push_back(DeltaTime());

		float front = pastDt.front();

		if (pastDt.size() > 3 / iw::Time::DeltaTime()) {
			pastDt.erase(pastDt.begin());
		}

		float total = smoothDeltaTime * pastDt.size();
		total -= front;
		total += DeltaTime();

		smoothDeltaTime = total / pastDt.size();
	}

	size_t Ticks() {
		return ticks;
	}

	float TotalTime() {
		return time;
	}

	float RawDeltaTime() {
		return deltaTime.count() / 1000000000.0f;
	}

	float DeltaTime() {
		return RawDeltaTime() * TimeScale();
	}

	float SmoothDeltaTime()
	{
		return smoothDeltaTime;
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

	std::pair<float*, size_t> Times() {
		return std::make_pair<float*, size_t>(&pastDt.front(), pastDt.size());
	}
}
}
