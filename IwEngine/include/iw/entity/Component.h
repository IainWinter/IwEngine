#pragma once

#include "IwEntity.h"
#include <initializer_list>
#include <functional>
#include <typeindex>
#include <vector>

namespace iw {
namespace ECS {
	using func_DeepCopy = std::function<void(void*, void*)>;

	struct IWENTITY_API Component {
#ifdef IW_USE_REFLECTION
		const iw::Type* Type;
#else
		size_t Type;
#endif
		size_t Size;
		const char* Name;

		size_t Id;

		func_DeepCopy DeepCopyFunc;

		static inline size_t Hash(
			std::initializer_list<iw::ref<Component>> components)
		{
			return Hash(components.begin(), components.end());
		}

		template<
			typename _itr>
		static size_t Hash(
			_itr begin,
			_itr end)
		{
			size_t seed = std::distance(begin, end);
			for (_itr component = begin; component != end; component++) {
				seed ^= (size_t)(*component)->Name + 0x9e3779b9 + (seed << 6) + (seed >> 2);
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

	template<
		typename _t>
	func_DeepCopy GetCopyFunc() { // 8 squintillionzz iq
		return [](void* ptr, void* data) {
			_t* p = (_t*)ptr;
			_t* d = (_t*)data;

			new(p) _t(*d);
		};
	}
}

	using namespace ECS;
}
