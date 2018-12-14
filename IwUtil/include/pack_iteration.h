#pragma once

#include <cstddef>
#include <tuple>

namespace iwutil {
	template<
		std::size_t...>
	struct sum
	  : std::integral_constant<std::size_t, 0> {};

	template<
		std::size_t n,
		std::size_t... ns>
	struct sum<n, ns...>
	  : std::integral_constant<std::size_t, n + sum<ns...>::value> {};

	template<
		typename... _t>
	constexpr std::size_t sizeof_pack() {
		return sum<sizeof(_t)...>::value;
	}
}