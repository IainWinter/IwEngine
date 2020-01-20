#pragma once

#include "IwEntity.h"
#include <initializer_list>
#include <typeindex>
#include <vector>

namespace IW {
namespace ECS {
	using ComponentType = std::type_index;

	struct IWENTITY_API Component {
		size_t Type;
		size_t Size;
		const char* Name;

		static size_t Hash(
			std::initializer_list<iw::ref<Component>> components);

		template<
			typename _itr>
		static size_t Hash(
			_itr begin,
			_itr end)
		{
			size_t seed = std::distance(begin, end);
			for (_itr component = begin; component != end; component++) {
				seed ^= (*component)->Type + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}

			return seed;
		}
	};

	struct ComponentQuery {
		size_t Count;
		iw::ref<Component> Components[];

		//std::vector<iw::ref<Component>> All;
		//std::vector<iw::ref<Component>> None;
		//std::vector<iw::ref<Component>> Any;
	};
}

	using namespace ECS;
}
