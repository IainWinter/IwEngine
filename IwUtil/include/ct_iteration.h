#pragma once

#include <cstddef>
#include <tuple>

namespace iwutil {
	template<
		std::size_t...>
	struct sum
		: std::integral_constant<std::size_t, 0 > {};

	template<
		std::size_t n,
		std::size_t... ns>
	struct sum<n, ns...>
		: std::integral_constant<std::size_t, n + sum<ns...>::value> {};

	template<
		typename... _t>
	constexpr std::size_t sizeof_sum() {
		return sum<sizeof(_t)...>::value;
	}

	template<
		typename _functor_t,
		typename _tuple_t,
		std::size_t... _tuple_index,
		typename... _args_t>
	void foreach(
		_functor_t&& functor,
		_tuple_t& tuple,
		std::index_sequence<_tuple_index...>,
		const _args_t&... args)
	{
		auto e = { (functor(std::get<_tuple_index>(tuple), args...), 0)... };
	}

	template<
		typename _functor_t, 
		typename _tuple_t, 
		std::size_t _tuple_size,
		typename... _args_t>
	void foreach(
		_tuple_t& tuple,
		const _args_t&... args)
	{
		foreach(_functor_t(), tuple, std::make_index_sequence<_tuple_size>(), args...);
	}
}

namespace functors {
	struct increment {
		template<
			typename _t>
		void operator()(_t&& t) {
			t++;
		}
	};

	struct decrement {
		template<
			typename _t>
		void operator()(_t&& t) {
			t--;
		}
	};

	struct erase {
		template<
			typename _t>
		void operator()(_t&& t) {
			delete t;
		}
	};

	struct erase_array {
		template<
			typename _t>
		void operator()(_t&& t) {
			delete[] t;
		}
	};
}