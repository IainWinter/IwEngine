#pragma once

#include "IwInput.h"
#include <cstring>

namespace IW {
	inline namespace Input {
		class InputState {
		private:
			float states[INPUT_COUNT];
			bool  locks[3];

		public:
			InputState() {
				memset(states, (int)0.0f, sizeof(states));
				memset(locks,  (int)false, sizeof(locks));
			}

			inline const float* States() {
				return states;
			}

			inline float& operator[](
				int index)
			{
				return states[index];
			}

			inline bool& GetLock(
				InputName lock)
			{
				return locks[lock - CAPS_LOCK];
			}

			inline void ToggleLock(
				InputName lock)
			{
				bool& l = GetLock(lock);
				l = !l;
			}
		};
	}
}
