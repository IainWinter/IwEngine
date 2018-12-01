#pragma once

#ifdef IWECS_EXPORT
#	define IWECS_API __declspec(dllexport)
#else
#	define IWECS_API __declspec(dllimport)
#endif

#include <vector>

namespace iwecs {
	using entity_t = std::size_t;
	using index_t = std::size_t;

	struct ientity_data {
		index_t index;

		ientity_data(index_t index)
			: index(index) {
		}

		virtual ~ientity_data() {}
	};

	template<std::size_t _size>
	struct entity_data : ientity_data {
		const void* components[_size];

		entity_data()
		  : ientity_data(0) {}

		entity_data(
			index_t index,
			const void* components_[_size])
		  : ientity_data(index)
		{
			memcpy(components, components_, _size);
		}
	};
}