#pragma once

#include "IwEntity.h"
#include <initializer_list>
#include <typeindex>

namespace IwEntity {
	using ComponentType = std::type_index;

	struct Component {
		size_t Type;
		size_t Size;
		const char* Name;

		static size_t Hash(
			std::initializer_list<iwu::ref<const Component>> components);
	};
}
