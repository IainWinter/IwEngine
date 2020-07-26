#pragma once

#include "IwEntity.h"

namespace iw {
namespace ECS {
	struct ComponentData {
		void* Components[];

		template<
			typename _cs>
		_cs Tie() {
			return *reinterpret_cast<_cs*>(Components);
		}
	};

	struct ComponentDataIndices {
		size_t Count;
		size_t Indices[];
	};
}

	using namespace ECS;
}
