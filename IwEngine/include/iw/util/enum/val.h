#pragma once

#include <type_traits>

namespace iw {
namespace util {
	template<
		typename _e>
	constexpr auto val(
		_e e) noexcept
	{
		return static_cast<std::underlying_type_t<_e>>(e);
	}
}

	using namespace util;
}
