#pragma once

#include "IwUtil/iwutil.h"

namespace iwecs {
	template<typename... _components_t>
	struct archtype {
		static constexpr std::size_t size = iwutil::sizeof_all<_components_t...>();
	};


	std::size_t s = archtype<int, float>::size;
}