#pragma once

#include "IwEntity.h"

namespace IW {
namespace ECS {
	struct Entity {
		size_t Index;
		short  Version;
		bool   Alive;
	};
}

	using namespace ECS;
}
