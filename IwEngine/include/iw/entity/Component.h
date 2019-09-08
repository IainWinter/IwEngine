#pragma once

#include "IwEntity.h"
#include <initializer_list>
#include <memory>

namespace IwEntity {
	using ComponentType = std::type_index;

	struct Component {
		size_t Type;
		size_t Size;
		const char* Name;

		static size_t Hash(
			std::initializer_list<std::weak_ptr<Component>> components)
		{
			size_t seed = components.size();
			for (auto component : components) {
				if (auto c = component.lock()) {
					seed ^= c->Type + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				}
			}

			return seed;
		}
	};
}
