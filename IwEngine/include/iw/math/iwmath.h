#pragma once

#include "iw\reflection\Reflect.h"

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWMATH_API __declspec(dllexport)
#	else
#		define IWMATH_API __declspec(dllimport)
#	endif
#else
#	define IWMATH_API 
#endif

namespace iw {
namespace math {
	/**
	* Pi ~ 3.14
	*/
	constexpr float Pi = 3.14159265359f;

	/**
	* Pi ~ 6.28
	*/
	constexpr float Pi2 = Pi * 2;

	/**
	* e ~ 2.71
	*/
	constexpr float e = 2.71828182846f;

	/**
	* Phi ~ 1.62
	*/
	constexpr float Phi = 1.61803398875f;

	/**
	* Returns an approximation of the inverse
	*  square root of the specified value.
	*
	* @param value Value to inverse square root.
	*/
	IWMATH_API
	float fast_inv_sqrt(
		float value);

	/**
	* Returns an approximation of the \p a raised to the \p b
	*
	* @param a The base
	* @param b The exponent
	*/
	IWMATH_API
	float fast_pow(
		float base, float exponent);

	/**
	* Returns if 2 floating point numbers are equal to a specified accuracy.
	*
	* @param a A floating point number.
	* @param b A floating point number.
	* @param accuracy Number of decimals to compare.
	*/
	IWMATH_API
	bool almost_equal(
		float a, 
		float b, 
		unsigned int accuracy);

	/**
	* Returns the linear interpolation from a to b by w.
	*
	* @param a Value of origin.
	* @param b Value of destination.
	* @param w Percent through distance between points.
	* @tparam _t Any type that supports +_t, -_t, and *float operators.
	*/
	template<
		typename _t>
	_t lerp(
		const _t& a,
		const _t& b,
		float w)
	{
		return a + (b - a) * w;
	}
}

	using namespace math;
}
