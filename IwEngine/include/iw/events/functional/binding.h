#pragma once

#include <functional>
#include "../iwevents.h"

namespace iwevents {
	template<int N>
	struct placeholder { static placeholder ph; };

	template<int N>
	placeholder<N> placeholder<N>::ph;

	template<int N>
	struct std::is_placeholder<placeholder<N>>
		: std::integral_constant<int, N> { };

	template<
		typename _r,
		typename _t,
		typename... _args_t,
		typename _u,
		int... indices_>
	std::function<_r(_args_t...)> bind(
		std::function<_r(_t, _args_t...)> func,
		_u val,
		std::integer_sequence<int, indices_...>)
	{
		return std::bind(func, val, placeholder<indices_ + 1>::ph...);
	}

	template<
		typename _r,
		typename _t,
		typename... _args_t,
		typename _u>
	std::function<_r(_args_t...)> bind(
		std::function<_r(_t, _args_t...)> func,
		_u val)
	{
		return bind(func, val,
			std::make_integer_sequence<int, sizeof...(_args_t)>());
	}
}
