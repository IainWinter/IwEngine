#include "iw/entity/Component.h"

namespace IwEntity {
	size_t Component::Hash(
		std::initializer_list<iwu::ref<const Component>> components)
	{
		size_t seed = components.size();
		for (iwu::ref<const Component> component : components) {
			seed ^= component->Type + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		return seed;
	}
}
