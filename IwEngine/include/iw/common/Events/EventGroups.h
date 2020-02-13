#pragma once

#include "iw/util/enum/val.h"

namespace iw {
namespace Engine {
	enum class EventGroup
		: short
	{
		OS,
		WINDOW,
		INPUT,
		ACTION,
		PHYSICS,
		ENTITY,
		NOT_HANDLED
	};
}

	using namespace Engine;
}
