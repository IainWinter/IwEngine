#pragma once

#include "IwEntity.h"
#include <initializer_list>
#include <typeindex>
#include <vector>

namespace IwEntity {
	using ComponentType = std::type_index;

	struct Component {
		size_t Type;
		size_t Size;
		const char* Name;

		static size_t Hash(
			std::initializer_list<iwu::ref<const Component>> components);
	};

	struct ComponentQuery {
		std::vector<iwu::ref<const Component>> All;
		std::vector<iwu::ref<const Component>> None;
		std::vector<iwu::ref<const Component>> Any;
	};
}
