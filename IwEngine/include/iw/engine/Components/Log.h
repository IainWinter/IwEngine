#pragma once

#include "iw/math/vector3.h"

namespace IwEngine {
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
		iwm::vector3 Vector;
	};
}
