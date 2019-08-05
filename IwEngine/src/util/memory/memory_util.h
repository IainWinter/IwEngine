#pragma once

#include "iw/util/iwutil.h"

namespace iwutil {
	std::size_t calc_padding(
		std::size_t address,
		std::size_t alignment)
	{
		std::size_t multiplier = (address / alignment) + 1;
		std::size_t aligned    = multiplier* alignment;
		return aligned - address;
	}
}
