#pragma once

#include "iwinput.h"
#include <cstring>

namespace iw {
namespace Input {
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
			int input)
		{
			return GetState(input);
		}

		inline float& GetState(
			int input)
		{
			return states[input];
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

	using namespace Input;
}
