#include "iw/engine/Time.h"

namespace IwEngine {
	namespace Time {
		static Time time = std::chrono::high_resolution_clock::now();
		static Duration deltaTime = Duration::zero();
		static int ticks = 0;

		Time Update() {
			ticks++;
			deltaTime = std::chrono::high_resolution_clock::now() - time;
			time = std::chrono::high_resolution_clock::now();
			return time;
		}

		int Ticks() {
			return ticks;
		}

		double DeltaTime() {
			return deltaTime.count() / 1000000000.0;
		}
	}
}
