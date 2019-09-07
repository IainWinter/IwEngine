#pragma once

#include "IwEntity.h"
#include <initializer_list>

namespace IwEntity {
	using ComponentType = std::type_index;

	struct Component {
		size_t Type;
		size_t Size;
		const char* Name;

		static size_t Hash(
			std::initializer_list<Component> components)
		{
			size_t seed = components.size();
			for (auto c : components) {
				seed ^= c.Type + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}

			return seed;
		}
	};
}
