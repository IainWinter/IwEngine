#pragma once

#include "iwmath.h"
#include <ostream>

namespace iw {
namespace math {
	struct vector2;
	struct vector4;
	struct matrix3;
	struct matrix4;
	struct quaternion;

	/**
	* Represents a vector or point in 3D.
	*/
	struct REFLECT IWMATH_API vector3 {
		/**
		* A vector with components: 0, 0, 0.
		*/
		static const vector3 zero;

		/**
		* A vector with components: 1, 1, 1.
		*/
		static const vector3 one;

		/**
		* A vector with components: 1, 0, 0.
		*/
		static const vector3 unit_x;

		/**
		* A vector with components: 0, 1, 0.
		*/
		static const vector3 unit_y;

		/**
		* A vector with components: 0, 0, 1.
		*/
		static const vector3 unit_z;

		REFLECT float x;
		REFLECT float y;
		REFLECT float z;

		vector3();

		vector3(
			float xyz);

		vector3(
			const vector4& xy);

		vector3(
			const vector2& xy);

		vector3(
			const vector2& xy,
			float z);

		vector3(
			float x,
			float y,
			float z);

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
			const vector3& other) const;

		/**
		* Returns the cross product between the vector and another.
		*
		* @param other Vector to cross with.
		*/
		vector3 cross(
			const vector3& other) const;

		/**
		* Returns the normalized version of the vector.
		*/
		vector3 normalized() const;

		/**
		* Returns the normalized version of the vector using the
		*	inv_sqrt function. This is much faster, and accurate enough in most cases.
		*/
		vector3 normalized_fast() const;

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
		* Returns the most influential components
		*/
		float major() const;

		/**
		* Returns the lest influential components
		*/
		float minor() const;

		vector3 operator+ (
			const vector3& other) const;

		vector3 operator- (
			const vector3& other) const;

		vector3 operator* (
			const vector3& other) const;

		vector3 operator/ (
			const vector3& other) const;

		vector3 operator+=(
			const vector3& other);

		vector3 operator-=(
			const vector3& other);

		vector3 operator*=(
			const vector3& other);

		vector3 operator/=(
			const vector3& other);

		vector3 operator+(
			float other) const;

		vector3 operator-(
			float other) const;

		vector3 operator*(
			float other) const;

		vector3 operator/(
			float other) const;

		vector3 operator+=(
			float other);

		vector3 operator-=(
			float other);

		vector3 operator*=(
			float other);

		vector3 operator/=(
			float other);

		vector3 operator*(
			const matrix3& mat) const;

		vector3& operator*=(
			const matrix3& mat);

		//vector3 operator*(
		//	const matrix4& mat) const;

		//vector3& operator*=(
		//	const matrix4& mat);

		vector3 operator*(
			const quaternion& quat) const;

		vector3& operator*=(
			const quaternion& quat);

		vector3 operator-() const;

		float& operator[](
			size_t index);

		bool operator==(
			const vector3& other) const;

		bool operator!=(
			const vector3& other) const;

		IWMATH_API
		friend std::ostream& operator<<(
			std::ostream& stream,
			const vector3& vector);

		IWMATH_API
		friend vector3 operator+(
			float left,
			const vector3& right);

		IWMATH_API
		friend vector3 operator*(
			float left,
			const vector3& right);
	};
}

	using namespace math;
}
