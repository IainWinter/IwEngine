#pragma once

#include "IwEntity.h"

namespace IwEntity {
	struct ComponentData {
		size_t ChunkIndex;
		void*  Components[];
	};
}
