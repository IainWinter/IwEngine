#pragma once

#include "IwInput.h"
#include <cstring>

namespace IW {
inline namespace Input {
	class InputState {
	private:
		float states[INPUT_COUNT];

	public:
		InputState() {
			memset(states, (int)0.0f, INPUT_COUNT * sizeof(float));
		}

		inline const float* States() {
			return states;
		}

		float& operator[](int index) {
			return states[index];
		}
	};
}
}
