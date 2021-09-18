#pragma once

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IWMATH_DLL
#		define IWMATH_API __declspec(dllexport)
#	else
#		define IWMATH_API __declspec(dllimport)
#	endif
#else
#	define IWMATH_API 
#endif

#define NOMINMAX

#define GLM_CONFIG_XYZW_ONLY
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/norm.hpp"

namespace iw {
namespace math {

	// Forward decl matrix.h

	template<typename _t>
	struct matitr;

	struct matrix;

	template<size_t _height, size_t _width>
	struct mat;

	template<size_t _height>
	struct vec;


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

	template<typename _t>
	const _t& max(const _t& a, const _t& b)
	{
		return a >= b ? a : b;
	}

	template<typename _t>
	const _t& min(const _t& a, const _t& b)
	{
		return a <= b ? a : b;
	}


	/**
	* Returns a random float -1 - 1
	*/
	IWMATH_API
	float randfs();

	/**
	* Returns a random float 0 - 1
	*/
	IWMATH_API
	float randf();

	/**
	* Returns a random float 0 - max
	*/
	IWMATH_API
	int randi(
		int max = 1);

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

	/*
	* Returns 1 - (1 - x) * (1 - x).
	*
	* @param x A floating point number.
	*/
	IWMATH_API
	float ease(
		float time,
		float scale = 1.0f);

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
		// this still has a subtraction, but on a and b so the precision is in a/b hands not w

		return a + w * (b - a);

		// this is unstable when w has many decimals and a/b are =

		//if      (w < 0) w = 0;
		//else if (w > 1) w = 1;

		//return a * (1 - w) + b * w;
	}

	/**
	* Returns the the value clamped between min and max.
	*
	* @param a Value to be clamped.
	* @param min Min value of range.
	* @param max Max value of range.
	* @tparam _t Any type that implements '>' and '<' operators.
	*/
	template<
		typename _t> // should _t be a ref or not const refds
	_t clamp(
		const _t& x,
		const _t& min,
		const _t& max)
	{
		if (min > x) return min;
		if (max < x) return max;

		return x;
	}
}

	using namespace math;
}
