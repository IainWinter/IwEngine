#pragma once

#include "IwEntity.h"

namespace IW {
inline namespace ECS {
	struct Entity {
		size_t Index;
		short  Version;
		bool   Alive;
	};
}
}
