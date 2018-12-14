#pragma once

#include <cstddef>

namespace iwutil {
	template<typename>
	void type_id() {}

	using type_id_t = void(*)();
}