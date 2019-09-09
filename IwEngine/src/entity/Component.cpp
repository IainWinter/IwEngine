#include "iw/entity/Component.h"

namespace IwEntity {
	size_t Component::Hash(
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
}
