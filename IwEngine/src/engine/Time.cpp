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

		IWENGINE_API Tick Ticks() {
			return ticks;
		}

		Duration DeltaTime() {
			return std::chrono::high_resolution_clock::now() - time;
		}
	}
}
