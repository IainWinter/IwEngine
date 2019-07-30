#include "iw/engine/Time.h"

namespace IwEngine {
	namespace Time {
		static Time now          = std::chrono::high_resolution_clock::now();
		static Duration deltaTime = Duration::zero();
		static int ticks          = 0;
		static float time         = 0.0f;

		void Update() {
			ticks++;
			time += DeltaTime();
			deltaTime = std::chrono::high_resolution_clock::now() - now;
			now = std::chrono::high_resolution_clock::now();
		}

		int Ticks() {
			return ticks;
		}

		int TotalTime() {
			return time * 1000;
		}

		float DeltaTime() {
			return deltaTime.count() / 1000000000.0f;
		}
	}
}
