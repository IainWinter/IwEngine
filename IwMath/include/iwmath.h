#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_BUILD_DLL
#		define IWMATH_API __declspec(dllexport)
#else
#		define IWMATH_API __declspec(dllimport)
#	endif
#endif

namespace iwm {}
#define iwmath iwm

namespace iwmath {
	/**
	* Pi ~ 3.14
	*/
	constexpr float IW_PI = 3.14159265359f;

	/**
	* Pi ~ 6.28
	*/
	constexpr float IW_2PI = IW_PI * 2;

	/**
	* e ~ 2.71
	*/
	constexpr float IW_E = 2.71828182846f;

	/**
	* Phi ~ 1.62
	*/
	constexpr float IW_PHI = 1.61803398875f;

	/**
	* Returns the inverse square root of the specified value.
	*
	* @param value Value to inverse square root.
	*/
	IWMATH_API float inv_sqrt(float value);

	/**
	* Returns if 2 floating point numbers are equal to a specified accuracy.
	*
	* @param a A floating point number.
	* @param b A floating point number.
	* @param accuracy Number of decimals to compare.
	*/
	IWMATH_API bool almost_equal(float a, float b, unsigned int accuracy);
}