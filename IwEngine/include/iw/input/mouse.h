#pragma once

#include "input_device.h"

namespace iwinput {
	class IWINPUT_API mouse : public input_device {
	public:
		virtual ~mouse() {}

		virtual uint id() = 0;
		virtual float poll_state(uint state_id) = 0;
		virtual void recenter_on_update(bool recenter) = 0;
	};
}