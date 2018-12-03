#pragma once

#ifdef IWECS_EXPORT
#	define IWECS_API __declspec(dllexport)
#else
#	define IWECS_API __declspec(dllimport)
#endif

#include "IwUtil/type_id.h"

namespace iwecs {
	using entity_t = std::size_t;
	using index_t = std::size_t;

	struct ientity_data {
		index_t index;
		iwutil::type_id_t archetype_id;

		ientity_data(index_t index, iwutil::type_id_t archetype_id)
		  : index(index),
			archetype_id(archetype_id) {}

		virtual ~ientity_data() {}
	};

	template<std::size_t _size>
	struct entity_data : ientity_data {
		void* components[_size];

		entity_data()
		  : ientity_data(0, 0) {}

		entity_data(
			index_t index,
			iwutil::type_id_t archetype_id,
			void* components_[_size])
		  : ientity_data(index, archetype_id)
		{
			memcpy(components, components_, _size);
		}
	};
}