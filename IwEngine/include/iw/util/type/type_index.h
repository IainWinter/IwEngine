#pragma once

#include "iw/util/iwutil.h"

namespace iwutil {
	namespace detail
	{
		template <
			typename _t,
			std::size_t _n,
			typename... _args_t>
		struct type_index {
			static constexpr auto value = _n;
		};

		template <
			typename _t,
			std::size_t _n,
			typename... _args_t>
		struct type_index<
			_t,
			_n,
			_t,
			_args_t...>
		{
			static constexpr auto value = _n;
		};

		template <
			typename _t,
			std::size_t _n,
			typename _u,
			typename... _args_t>
		struct type_index<
			_t,
			_n,
			_u,
			_args_t...>
		{
			static constexpr auto value = type_index<_t, _n + 1, _args_t...>::value;
		};
	}

	template<
		typename _t,
		typename... _args_t>
	_t get_typeof(
		const std::tuple<_args_t...>& tuple)
	{
		return std::get<detail::type_index<_t, 0, _args_t...>::value>(tuple);
	}

}
