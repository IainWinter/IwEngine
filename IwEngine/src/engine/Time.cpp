#include "iw/engine/Time.h"

namespace IwEngine {
	namespace Time {
		static Time time = std::chrono::high_resolution_clock::now();
		static int ticks = 0;

		Time Update() {
			ticks++;
			time = std::chrono::high_resolution_clock::now();
			return time;
		}

		int Ticks() {
			return ticks;
		}

		double DeltaTime() {
			return (std::chrono::high_resolution_clock::now() - time).count()
				/ 1000000000.0;
		}
	}
}
