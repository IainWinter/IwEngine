#pragma once

#include "IwEntity.h"

namespace IwEntity {
	struct Entity {
		size_t Index;
		short  Version;
		bool   Alive;
	};
}
