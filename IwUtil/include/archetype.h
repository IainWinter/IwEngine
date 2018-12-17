#pragma once

#include <cstddef>
#include "pack_iteration.h"
#include "type_id.h"

namespace iwutil {
	template<
		typename... _t>
	struct archetype {
		//Number of types in the pack
		static constexpr std::size_t size
			= sizeof...(_t);

		//Sum of the size of each type
		static constexpr std::size_t size_in_bytes
			= iwutil::sizeof_pack<_t...>();

		//Sum of the id of each type
		static constexpr std::size_t id
			= iwutil::type_id::value<_t...>();
	};
}