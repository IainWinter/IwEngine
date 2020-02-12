#pragma once

#include "iw/math/vector3.h"

#undef ERROR

namespace iw {
	enum LogType {
		INFO,
		DEBUG,
		WARNNING,
		ERROR
	};

	struct Log {
		LogType Type;
	};

	struct VectorLog
		: Log
	{
		iw::vector3 Vector;
	};
}
