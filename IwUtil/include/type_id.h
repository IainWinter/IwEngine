#pragma once

#include <cstddef>

namespace iwutil {
	using typeid_t = void(*)();

	template <typename _t>
	typeid_t type_id() noexcept {
		return typeid_t(type_id<_t>);
	}
}