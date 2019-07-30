#include "iw/engine/Time.h"
#include "iw/log/logger.h"

namespace IwEngine {
	namespace Time {
		static Time now           = std::chrono::high_resolution_clock::now();
		static Duration deltaTime = Duration::zero();
		static int ticks          = 0;
		static float time         = 0.0f;
		static float fixedTime    = 0.2f;

		void SetFixedTime(
			float duration) 
		{
			fixedTime = duration;
			LOG_INFO << "Set fixed timeset to " << duration << " seconds";
		}

		void Update() {
			ticks++;
			time += DeltaTime();
			deltaTime = std::chrono::high_resolution_clock::now() - now;
			now = std::chrono::high_resolution_clock::now();
		}

		int Ticks() {
			return ticks;
		}

		float TotalTime() {
			return time * 1000;
		}

		float DeltaTime() {
			return deltaTime.count() / 1000000000.0f;
		}

		float FixedTime() {
			return fixedTime;
		}
	}
}
