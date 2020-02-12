#pragma once

#include <type_traits>

namespace iw {
namespace RenderAPI {
	enum class UniformType {
		BOOL,
		INT,
		UINT,
		FLOAT,
		DOUBLE,
		SAMPLE2,
		UNKNOWN
	};

	template<
		typename _t>
	constexpr UniformType GetUniformType() {
		if constexpr(std::is_same_v<_t, bool>)     return UniformType::BOOL;
		if constexpr(std::is_same_v<_t, int>)      return UniformType::INT;
		if constexpr(std::is_same_v<_t, unsigned>) return UniformType::UINT;
		if constexpr(std::is_same_v<_t, float>)    return UniformType::FLOAT;
		if constexpr(std::is_same_v<_t, double>)   return UniformType::DOUBLE;

		return UniformType::UNKNOWN;
	}
}
}
