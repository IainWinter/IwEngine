#include "iw/entity/Component.h"

namespace IW {
	size_t Component::Hash(
		std::initializer_list<iw::ref<Component>> components)
	{
		size_t seed = components.size();
		for (iw::ref<Component> component : components) {
			seed ^= component->Type + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		return seed;
	}
}
