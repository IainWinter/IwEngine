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
			std::initializer_list<iwu::ref<Component>> components);
	};

	struct ComponentQuery {
		size_t Count;
		iwu::ref<Component> Components[];

		//std::vector<iwu::ref<Component>> All;
		//std::vector<iwu::ref<Component>> None;
		//std::vector<iwu::ref<Component>> Any;
	};
}
