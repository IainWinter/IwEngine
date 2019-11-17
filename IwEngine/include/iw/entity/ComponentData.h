#pragma once

#include "IwEntity.h"

namespace IW {
inline namespace ECS {
	struct ComponentData {
		void* Components[];

		template<
			typename _cs>
		_cs Tie() {
			return *reinterpret_cast<_cs*>(Components);
		}

		template<
			typename... _cs>
		std::tuple<_cs*...> TieTo() {
			return *reinterpret_cast<std::tuple<_cs*...>*>(Components);
		}
	};

	struct ComponentDataIndices {
		size_t Count;
		size_t Indices[];
	};
}
}
