#pragma once

#include <tuple>

namespace iwutil {
	template<
		typename _functor,
		typename _tuple,
		typename _arg_tuple,
		typename std::size_t... _index>
	void foreach(
		_functor&& functor,
		_tuple& tuple,
		_arg_tuple args,
		std::index_sequence<_index...>)
	{
		auto e = {(
			functor(std::get<_index>(tuple), std::get<_index>(args)),
			0)...
		};
	}
}