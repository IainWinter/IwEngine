#pragma once

#include "iw/util/iwutil.h"
#include <cstddef>
#include <tuple>

namespace iwutil {
	template<
		class _t,
		class _tuple>
	struct index;

	template<
		class _t,
		class _type>
	struct index<
		_t,
		std::tuple<_type>>
	{
		static_assert(std::is_same_v<_t, _type>, "Type not found!");

		static const std::size_t value = 0;
	};

	template<
		class _t,
		class... _types>
	struct index<
		_t,
		std::tuple<_t, _types...>>
	{
		static const std::size_t value = 0;
	};

	template<
		class _t,
		class _u,
		class... _types>
	struct index<
		_t,
		std::tuple<_u, _types...>>
	{
		static const std::size_t value = 1
			+ index<_t, std::tuple<_types...>>::value;
	};
}
