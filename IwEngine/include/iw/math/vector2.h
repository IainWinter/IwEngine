#pragma once

#include "iwmath.h"
#include <ostream>

namespace iw {
namespace math {
	struct vector3;
	struct vector4;

	/**
	* Represents a vector or point in 2D.
	*/
	struct REFLECT IWMATH_API vector2 {
		/**
		* A vector with components: 0, 0.
		*/
		static const vector2 zero;

		/**
		* A vector with components: 1, 1.
		*/
		static const vector2 one;

		/**
		* A vector with components: 1, 0.
		*/
		static const vector2 unit_x;

		/**
		* A vector with components: 0, 1.
		*/
		static const vector2 unit_y;

		REFLECT float x;
		REFLECT float y;

		vector2();

		vector2(
			float xy);

		vector2(
			const vector3& xy);

		vector2(
			const vector4& xy);

		vector2(
			float x,
			float y);

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
			const vector2& other) const;

		/**
		* Returns the length of the would be vector created 
		*	from a cross product in 3D.
		*
		* @param other Vector to cross with.
		*/
		float cross_length(
			const vector2& other) const;

		/**
		* Returns the normalized version of the vector.
		*/
		vector2 normalized() const;

		/**
		* Returns the normalized version of the vector 
		*	using the inv_sqrt function. This is much faster, 
		*	and accurate enough in most cases.
		*/
		vector2 normalized_fast() const;

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

		vector2 operator+ (
			const vector2& other) const;

		vector2 operator- (
			const vector2& other) const;

		vector2 operator* (
			const vector2& other) const;

		vector2 operator/ (
			const vector2& other) const;

		vector2 operator+=(
			const vector2& other);

		vector2 operator-=(
			const vector2& other);

		vector2 operator*=(
			const vector2& other);

		vector2 operator/=(
			const vector2& other);

		vector2 operator+ (
			float other) const;

		vector2 operator- (
			float other) const;

		vector2 operator* (
			float other) const;

		vector2 operator/ (
			float other) const;

		vector2 operator+=(
			float other);

		vector2 operator-=(
			float other);

		vector2 operator*=(
			float other);

		vector2 operator/=(
			float other);

		vector2 operator-() const;

		float& operator[](
			size_t index);

		bool operator==(
			const vector2& other) const;

		bool operator!=(
			const vector2& other) const;

		IWMATH_API
		friend std::ostream& operator<<(
			std::ostream& stream,
			const vector2& vector);

		IWMATH_API
		friend vector2 operator+(
			float left,
			const vector2& right);

		IWMATH_API
		friend vector2 operator*(
			float left,
			const vector2& right);

		inline static vector2 random() {
			return vector2(randf(), randf()).normalized();
		}
	};
}

	using namespace math;
}
