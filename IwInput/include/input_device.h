#pragma once

#include "iwinput.h"

namespace iwinput {
	class IWINPUT_API input_device {
	public:
		virtual ~input_device() {}

		virtual uint id() = 0;
		virtual float poll_state(uint state_id) = 0;
	};
}