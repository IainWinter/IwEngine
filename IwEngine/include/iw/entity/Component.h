#pragma once

#include "IwEntity.h"
#include <initializer_list>
#include <typeindex>
#include <vector>

namespace IW {
inline namespace ECS {
	using ComponentType = std::type_index;

	struct IWENTITY_API Component {
		size_t Type;
		size_t Size;
		const char* Name;

		static size_t Hash(
			std::initializer_list<iw::ref<Component>> components);
	};

	struct ComponentQuery {
		size_t Count;
		iw::ref<Component> Components[];

		//std::vector<iw::ref<Component>> All;
		//std::vector<iw::ref<Component>> None;
		//std::vector<iw::ref<Component>> Any;
	};
}
}
