#pragma once

#include <cstddef>
#include "ct_iteration.h"
#include "type_id.h"

namespace iwutil {
	template<typename... _components_t>
	struct archtype {
		static constexpr std::size_t size          = sizeof...(_components_t);
		static constexpr std::size_t size_in_bytes = iwutil::sizeof_sum<_components_t...>();
		static constexpr iwutil::type_id_t id      = iwutil::type_id<archtype<_components_t...>>;
	};
}