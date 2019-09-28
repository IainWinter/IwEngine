#pragma once

#include "IwEntity.h"

namespace IwEntity {
	struct ComponentData {
		size_t ChunkIndex;
		void*  Components[];

		template<
			typename _cs>
		_cs Tie() {
			return *reinterpret_cast<_cs*>(Components);
		}
	};
}
