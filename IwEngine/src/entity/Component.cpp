#include "iw/entity/Component.h"

namespace iw {
namespace ECS {
	size_t Component::Hash(
		std::initializer_list<iw::ref<Component>> components)
	{
		return Hash(components.begin(), components.end());
	}
}
}
