#pragma once

#include "iwmath.h"
#include <ostream>

namespace iw {
namespace math {
	struct vector2;
	struct vector3;
	struct matrix4;

	/**
	* Represents a vector with 4 dimensions.
	*/
	struct REFLECT IWMATH_API vector4 {
		/**
		* A vector with components: 0, 0, 0, 0.
		*/
		static const vector4 zero;

		/**
		* A vector with components: 1, 1, 1, 1.
		*/
		static const vector4 one;

		/**
		* A vector with components: 1, 0, 0, 0.
		*/
		static const vector4 unit_x;

		/**
		* A vector with components: 0, 1, 0, 0.
		*/
		static const vector4 unit_y;

		/**
		* A vector with components: 0, 0, 1, 0.
		*/
		static const vector4 unit_z;

		/**
		* A vector with components: 0, 0, 0, 1.
		*/
		static const vector4 unit_w;

		REFLECT float x;
		REFLECT float y;
		REFLECT float z;
		REFLECT float w;

		vector4();

		vector4(
			float xyzw);

		vector4(
			const vector2& xy);

		vector4(
			const vector2& xy,
			float z,
			float w);

		vector4(
			const vector3& xyz);

		vector4(
			const vector3& xyz,
			float w);

		vector4(
			float x,
			float y,
			float z,
			float w);

		/**
		* Returns the length of the vector.
		*/
		float length() const;

		/**
		* Returns the squared length of the vector.
		*	Use this for length comparisons.
		*/
		float length_squared() const;

		/**
		* Returns the length of the vector using the inv_sqrt function.
		*	This is much faster, and accurate enough in most cases.
		*/
		float length_fast() const;

		/**
		* Returns the dot product between the vector and another.
		*
		* @param other Vector to dot with.
		*/
		float dot(
			const vector4& other) const;

		/**
		* Returns the normalized version of the vector.
		*/
		vector4 normalized() const;

		/**
		* Returns the normalized version of the vector using the inv_sqrt function.
		*	This is much faster, and accurate enough in most cases.
		*/
		vector4 normalized_fast() const;

		/**
		* Normalizes the vector.
		*/
		void normalize();

		/**
		* Normalizes the vector using the inv_sqrt function.
		*	This is much faster, and accurate enough in most cases.
		*/
		void normalize_fast();

		/**
		* Returns the x, y, and z components in a vector3.
		*/
		vector3 xyz() const;

		/**
		* Sets the x, y, and z components in the vector4.
		*/
		void xyz(
			const vector3& xyz);

		/**
		* Sets the x, y, and z components in the vector4.
		*/
		void xyz(
			float x,
			float y,
			float z);

		vector4 operator+(
			const vector4& other) const;

		vector4 operator-(
			const vector4& other) const;

		vector4 operator*(
			const vector4& other) const;

		vector4 operator/(
			const vector4& other) const;

		vector4& operator+=(
			const vector4& other);

		vector4& operator-=(
			const vector4& other);

		vector4& operator*=(
			const vector4& other);

		vector4& operator/=(
			const vector4& other);

		vector4 operator+(
			const float& other) const;

		vector4 operator-(
			const float& other) const;

		vector4 operator*(
			const float& other) const;

		vector4 operator/(
			const float& other) const;

		vector4 operator+=(
			const float& other);

		vector4& operator-=(
			const float& other);

		vector4& operator*=(
			const float& other);

		vector4& operator/=(
			const float& other);

		vector4 operator*(
			const matrix4& mat) const;

		vector4& operator*=(
			const matrix4& mat);

		vector4 operator-() const;

		float& operator[](
			size_t index);

		bool operator==(
			const vector4& other) const;

		bool operator!=(
			const vector4& other) const;

		IWMATH_API
		friend std::ostream& operator<<(
			std::ostream& stream,
			const vector4& vector);

		IWMATH_API
		friend vector4 operator+(
			float left,
			const vector4& right);

		IWMATH_API
		friend vector4 operator*(
			float left,
			const vector4& right);
	};
}

	using namespace math;
}
