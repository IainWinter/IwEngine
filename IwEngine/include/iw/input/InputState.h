#pragma once

#include "iwinput.h"
#include <cstring>

namespace iw {
namespace Input {
	class InputState {
	private:
		float states[INPUT_COUNT];
		bool  locks[3]; // caps, numb, scroll

	public:
		InputState()
		{
			Reset();
		}

		const float* States() {
			return states;
		}

		float& operator[](
			int input)
		{
			return GetState(input);
		}

		float& GetState(
			int input)
		{
			return states[input];
		}

		bool& GetLock(
			InputName lock)
		{
			return locks[lock - CAPS_LOCK];
		}

		void ToggleLock(
			InputName lock)
		{
			bool& l = GetLock(lock);
			l = !l;
		}

		void Reset()
		{
			for (float& state : states) state = 0.f;
			for (bool&  state : locks)  state = false;
		}
	};
}

	using namespace Input;
}
