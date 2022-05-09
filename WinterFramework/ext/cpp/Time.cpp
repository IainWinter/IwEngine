#include "../Time.h"
#include <chrono>
#include <vector>

namespace Time
{
	static auto now        = std::chrono::high_resolution_clock::now();
	static auto deltaTime  = std::chrono::high_resolution_clock::duration::zero();
	static size_t ticks    = 0;
	static float time      = 0.0f;
	static float rawtime   = 0.0f;
	static float fixedTime = 0.2f;
	static float timeScale = 1.0f;

	static std::vector<float> pastDt;
	float smoothDeltaTime;

	void SetDeltaTime(float dt)
	{
		deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<float, std::ratio<1, 1>>(dt));
	}

	void SetFixedTime(float duration) { fixedTime = duration; }
	void SetTimeScale(float duration) { timeScale = duration; }

	void UpdateTime()
	{
		ticks++;
		time += DeltaTime();
		rawtime += RawDeltaTime();
		deltaTime = std::chrono::high_resolution_clock::now() - now;
		now = std::chrono::high_resolution_clock::now();

		pastDt.push_back(DeltaTime());

		float front = pastDt.front();

		if (pastDt.size() > 3 / Time::DeltaTime()) {
			pastDt.erase(pastDt.begin());
		}

		float total = smoothDeltaTime * pastDt.size();
		total -= front;
		total += DeltaTime();

		smoothDeltaTime = total / pastDt.size();
	}

	uint64_t Ticks() { return ticks; }
	float TotalTime() { return time; }
	float RawTotalTime() { return rawtime; }
	float RawDeltaTime() { return deltaTime.count() / 1000000000.0f; }
	float DeltaTime() { return RawDeltaTime() * TimeScale(); }
	float SmoothDeltaTime() { return smoothDeltaTime; }
	float DeltaTimeNow() { return (std::chrono::high_resolution_clock::now() - now).count() / 1000000000.0f; }
	float RawFixedTime() { return fixedTime; }
	float FixedTime() { return RawFixedTime() * TimeScale(); }
	float TimeScale() { return timeScale; }
	std::pair<float*, uint64_t> Times() { return { &pastDt.front(), pastDt.size() }; }
}