#pragma once

#include "iwmath.h"
#include "vector2.h"
#include <ostream>

namespace iwmath {
	/**
	* Represents a 2x2 matrix.
	*/
	struct IWMATH_API matrix2 {
		/**
		* 2x2 Identity matrix
		*/
		static const matrix2 identity;

		union {
			float elements[2 * 2];
			vector2 rows[2];
		};

		matrix2();

		matrix2(
			float diagonal);

		matrix2(
			float* elements);

		matrix2(
			vector2 row0,
			vector2 row1);

		matrix2(
			float m00, float m01,
			float m10, float m11
		);

		/**
		* Returns the determinant of the martix.
		*/
		float determinant() const;

		/**
		* Returns the trace of the martix.
		*/
		float trace() const;

		/**
		* Returns the transaposed version of the matrix.
		*/
		matrix2 transposed() const;

		/**
		* Returns the inverted version of the matrix.
		*/
		matrix2 inverted() const;

		/**
		* Returns the normalized version of the matrix.
		*/
		matrix2 normalized() const;

		/**
		* Sets the martix to is transposed self.
		*/
		void transpose();

		/**
		* Sets the matrix to its inversed self.
		*/
		void invert();

		/**
		* Sets the martix to its normalized self.
		*/
		void normalize();

		matrix2 operator+ (
			const matrix2& other) const;

		matrix2 operator- (
			const matrix2& other) const;

		matrix2 operator* (
			const matrix2& other) const;

		matrix2 operator+=(
			const matrix2& other);

		matrix2 operator-=(
			const matrix2& other);

		matrix2 operator*=(
			const matrix2& other);

		matrix2 operator+ (
			float other) const;

		matrix2 operator- (
			float other) const;

		matrix2 operator* (
			float other) const;

		matrix2 operator/ (
			float other) const;

		matrix2 operator+=(
			float other);

		matrix2 operator-=(
			float other);

		matrix2 operator*=(
			float other);

		matrix2 operator/=(
			float other);

		matrix2 operator-() const;

		float& operator()(
			int x,
			int y);

		const float& operator()(
			int x,
			int y) const;

		bool operator==(
			const matrix2& other) const;

		bool operator!=(
			const matrix2& other) const;

		/**
		* Returns a matrix with a specified rotation.
		*
		* @param angle Rotation in radians.
		*/
		static matrix2 create_rotation(
			float angle);

		/**
		* Returns a matrix with a specified scale.
		*
		* @param scale Scale in the x and y dimensions.
		*/
		static matrix2 create_scale(
			float scale);

		/**
		* Returns a matrix with a specified scale.
		*
		* @param scale Scale in the x and y dimensions as a vector2.
		*/
		static matrix2 create_scale(
			const vector2& scale);

		/**
		* Returns a matrix with a specified scale.
		*
		* @param x Scale in the x dimension.
		* @param y Scale in the y dimension.
		*/
		static matrix2 create_scale(
			float x,
			float y);

		friend IWMATH_API std::ostream& operator<<(
			std::ostream& stream,
			const matrix2& matrix);
	};
}
