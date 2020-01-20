#include "iw/entity/Component.h"

namespace IW {
	size_t Component::Hash(
		std::initializer_list<iw::ref<Component>> components)
	{
		return Hash(components.begin(), components.end());
	}
}
