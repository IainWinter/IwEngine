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

		template<
			typename... _cs>
		std::tuple<_cs...> Tie() {
			return *reinterpret_cast<std::tuple<_cs...>*>(Components);
		}
	};
}
