#pragma once

#include "iw/util/enum/val.h"

namespace IW {
inline namespace Engine {
	enum class EventGroup
		: short
	{
		OS,
		WINDOW,
		INPUT,
		ACTION,
		NOT_HANDLED
	};
}
}
