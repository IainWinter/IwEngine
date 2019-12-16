#pragma once

#include "iw/events/iwevents.h"
#include <functional>

namespace iw {
namespace events {
namespace impl {
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
		int... indices_>
	std::function<_r(_args_t...)> bind(
		std::function<_r(_t, _args_t...)> func,
		_t val,
		std::integer_sequence<int, indices_...>)
	{
		return std::bind(func, val, placeholder<indices_ + 1>::ph...);
	}
}
}

namespace events {
	template<
		typename _r,
		typename _t,
		typename... _args_t>
	std::function<_r(_args_t...)> bind(
		std::function<_r(_t, _args_t...)> func,
		_t val)
	{
		return events::impl::bind(func, val,
			std::make_integer_sequence<int, sizeof...(_args_t)>());
	}

	template<
		typename _r,
		typename _t>
	std::function<_r()> bind(
		std::function<_r(_t)> func,
		_t val)
	{
		return std::bind(func, val);
	}
}

	using namespace events;
}
