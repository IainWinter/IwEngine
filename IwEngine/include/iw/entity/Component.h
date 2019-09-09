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
			std::initializer_list<std::weak_ptr<Component>> components);
	};
}
