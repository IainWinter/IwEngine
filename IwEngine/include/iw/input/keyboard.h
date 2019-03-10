#pragma once

#include "input_device.h"

namespace iwinput {
	class IWINPUT_API keyboard : public input_device {
	public:
		virtual ~keyboard() {}

		virtual uint id() = 0;
		virtual float poll_state(uint state_id) = 0;
	};
}