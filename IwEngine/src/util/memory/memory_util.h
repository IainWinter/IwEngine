#pragma once

#include "iw/util/iwutil.h"

namespace iw {
	size_t calc_padding(
		size_t address,
		size_t alignment)
	{
		size_t multiplier = (address / alignment) + 1;
		size_t aligned    = multiplier* alignment;
		return aligned - address;
	}
}
