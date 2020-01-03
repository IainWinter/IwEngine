#pragma once

#include "iw/util/enum/val.h"

namespace IW {
namespace Engine {
	enum class EventGroup
		: short
	{
		OS,
		WINDOW,
		INPUT,
		ACTION,
		PHYSICS,
		NOT_HANDLED
	};
}

	using namespace Engine;
}
