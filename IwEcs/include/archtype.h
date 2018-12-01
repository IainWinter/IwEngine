#pragma once

#include "IwUtil/iwutil.h"

namespace iwecs {
	template<typename... _components_t>
	struct archtype {
		static constexpr std::size_t size = sizeof...(_components_t);
		static constexpr std::size_t size_in_bytes = iwutil::sizeof_all<_components_t...>();
	};
}