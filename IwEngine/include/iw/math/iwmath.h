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
	constexpr float PI = 3.14159265359f;

	/**
	* Pi ~ 6.28
	*/
	constexpr float PI2 = PI * 2;

	/**
	* e ~ 2.71
	*/
	constexpr float E = 2.71828182846f;

	/**
	* Phi ~ 1.62
	*/
	constexpr float PHI = 1.61803398875f;

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
}

	using namespace math;
}
