#pragma once

#ifdef IW_USE_REFLECTION
#	include "iw\reflection\Reflect.h"
#else
#	define REFLECT 
#endif

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
	struct vector2;
	struct vector3;
	struct vector4;

	/**
	* Pi ~ 3.14
	*/
	constexpr float Pi = 3.14159265359f;

	/**
	* 2 * Pi ~ 6.28
	*/
	constexpr float Pi2 = Pi * 2;

	/**
	* Pi / 2 ~ 1.57
	*/
	constexpr float hPi = Pi * 0.5f;

	/**
	* e ~ 2.71
	*/
	constexpr float e = 2.71828182846f;

	/**
	* Phi ~ 1.62
	*/
	constexpr float Phi = 1.61803398875f;

	/**
	* Returns a random float 0 - 1
	*/
	IWMATH_API
	float randf();

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
		if      (w < 0) w = 0;
		else if (w > 1) w = 1;

		return a * (1 - w) + b * w;
	}

	/**
	* Returns tje the value clamped inbetween min and max.
	*
	* @param a Value to be clamped.
	* @param min Min value of range.
	* @param max Max value of range.
	* @tparam _t Any type that is specialized.
	*/
	template<
		typename _t>
	_t clamp(
		const _t& x,
		const _t& min,
		const _t& max)
	{
		static_assert("no impl for clamp of type t");
	}

	template<>
	inline float clamp(
		const float& x,
		const float& min,
		const float& max)
	{
		if (min > x) return min;
		if (max < x) return max;

		return x;
	}
}

	using namespace math;
}
