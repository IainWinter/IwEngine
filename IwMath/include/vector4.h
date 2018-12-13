#pragma once

#include <ostream>
#include "iwmath.h"
#include "vector3.h"

namespace iwmath {
	/**
	* Represents a vector with 4 dimensions.
	*/
	struct IWMATH_API vector4 {
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

		float x, y, z, w;

		vector4();
		vector4(float xyzw);
		vector4(const vector3& xyz);
		vector4(const vector3& xyz, float w);
		vector4(float x, float y, float z, float w);

		/**
		* Returns the length of the vector.
		*/
		float length() const;

		/**
		* Returns the squared length of the vector. Use this for length comparisons.
		*/
		float length_squared() const;

		/**
		* Returns the length of the vector using the inv_sqrt function. This is much faster, and accurate enough in most cases.
		*/
		float length_fast() const;

		/**
		* Returns the dot product between the vector and another.
		*
		* @param other Vector to dot with.
		*/
		float dot(const vector4& other) const;

		/**
		* Returns the normalized version of the vector.
		*/
		vector4 normalized() const;

		/**
		* Returns the normalized version of the vector using the inv_sqrt function. This is much faster, and accurate enough in most cases.
		*/
		vector4 normalized_fast() const;

		/**
		* Normalizes the vector.
		*/
		void normalize();

		/**
		* Normalizes the vector using the inv_sqrt function. This is much faster, and accurate enough in most cases.
		*/
		void normalize_fast();

		/**
		* Returns the x, y, and z components in a vector3.
		*/
		vector3 xyz() const;

		/**
		* Sets the x, y, and z components in the vector4.
		*/
		void xyz(const vector3& xyz);

		/**
		* Sets the x, y, and z components in the vector4.
		*/
		void xyz(float x, float y, float z);

		vector4 operator+ (const vector4& other) const;
		vector4 operator- (const vector4& other) const;
		vector4 operator* (const vector4& other) const;
		vector4 operator/ (const vector4& other) const;
		vector4 operator+=(const vector4& other);
		vector4 operator-=(const vector4& other);
		vector4 operator*=(const vector4& other);
		vector4 operator/=(const vector4& other);
		vector4 operator+ (const float& other) const;
		vector4 operator- (const float& other) const;
		vector4 operator* (const float& other) const;
		vector4 operator/ (const float& other) const;
		vector4 operator+=(const float& other);
		vector4 operator-=(const float& other);
		vector4 operator*=(const float& other);
		vector4 operator/=(const float& other);
		vector4 operator-() const;
		float& operator[](std::size_t index);
		bool operator==(const vector4& other) const;
		bool operator!=(const vector4& other) const;

		friend IWMATH_API std::ostream& operator<<(std::ostream& stream, const vector4& vector);
		friend IWMATH_API vector4 operator+(float left, const vector4& right);
		friend IWMATH_API vector4 operator*(float left, const vector4& right);
	};
}