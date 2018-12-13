#pragma once

#include <ostream>
#include "iwmath.h"
#include "vector3.h"
#include "quaternion.h"

namespace iwmath {
	/**
	* Represents a 3x3 matrix.
	*/
	struct IWMATH_API matrix3 {
		/**
		* 3x3 Identity matrix
		*/
		static const matrix3 identity;

		union {
			float elements[3 * 3];
			vector3 rows[3];
		};

		matrix3();
		matrix3(float diagonal);
		matrix3(float* elements);
		matrix3(vector3 row0, vector3 row1, vector3 row2);
		matrix3(
			float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22
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
		matrix3 transposed() const;

		/**
		* Returns the inverted version of the matrix.
		*/
		matrix3 inverted() const;

		/**
		* Returns the normalized version of the matrix.
		*/
		matrix3 normalized() const;

		/**
		* Returns a version of the matrix with no rotation.
		*/
		matrix3 cleared_rotation() const;

		/**
		* Returns a version of the matrix with no scale.
		*/
		matrix3 cleared_scale() const;

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

		/**
		* Clears the roation from the matrix.
		*/
		void clear_rotation();

		/**
		* Clears the scale from the matrix.
		*/
		void clear_scale();

		/**
		* Returns the rotation of the matrix as a quaternion.
		*/
		quaternion rotation() const;

		/**
		* Returns the axis and angle of rotation. X, Y, Z is the axis, and W is the angle.
		*/
		vector4 axis_angle() const;

		/**
		* Returns the euler angle equivalent of the rotation.
		*/
		vector3 euler_angles() const;

		/**
		* Returns the scale of the matrix.
		*/
		vector3 scale() const;

		matrix3 operator+ (const matrix3& other) const;
		matrix3 operator- (const matrix3& other) const;
		matrix3 operator* (const matrix3& other) const;
		matrix3 operator+=(const matrix3& other);
		matrix3 operator-=(const matrix3& other);
		matrix3 operator*=(const matrix3& other);
		matrix3 operator+ (float other) const;
		matrix3 operator- (float other) const;
		matrix3 operator* (float other) const;
		matrix3 operator/ (float other) const;
		matrix3 operator+=(float other);
		matrix3 operator-=(float other);
		matrix3 operator*=(float other);
		matrix3 operator/=(float other);
		matrix3 operator-() const;
		float& operator()(int row, int col);
		bool operator==(const matrix3& other) const;
		bool operator!=(const matrix3& other) const;

		/**
		* Returns a matrix with a specified rotation around an axis.
		*
		* @param axisAngle Axis and angle of rotation in one vector.
		*/
		static matrix3 create_from_axis_angle(const vector4& axisAngle);

		/**
		* Returns a matrix with a specified rotation around an axis.
		*
		* @param axis Axis vector
		* @param angle Angle of rotation around the axis in radians.
		*/
		static matrix3 create_from_axis_angle(const vector3& axis, float angle);

		/**
		* Returns a matrix with a specified rotation around an axis.
		*
		* @param x X component of the axis vector.
		* @param y Y component of the axis vector.
		* @param z X component of the axis vector.
		* @param angle Angle of rotation around the axis in radians.
		*/
		static matrix3 create_from_axis_angle(float x, float y, float z, float angle);

		/**
		* Returns a matrix with a specified rotation.
		*
		* @param quaternion Quaternion to create matrix from.
		*/
		static matrix3 create_from_quaternion(const quaternion& quaternion);

		/**
		* Returns a matrix with a specified rotation around the x axis.
		*
		* @param angle Angle around the x axis in radians.
		*/
		static matrix3 create_rotation_x(float angle);

		/**
		* Returns a matrix with a specified rotation around the y axis.
		*
		* @param angle Angle around the y axis in radians.
		*/
		static matrix3 create_rotation_y(float angle);

		/**
		* Returns a matrix with a specified rotation around the z axis.
		*
		* @param angle Angle around the z axis in radians.
		*/
		static matrix3 create_rotation_z(float angle);

		/**
		* Returns a matrix with a specified rotation.
		*
		* @param angles Euler angles in radians.
		*/
		static matrix3 create_rotation(const vector3& angles);

		/**
		* Returns a matrix with a specified rotation.
		*
		* @param x Angle around the x axis in radians.
		* @param y Angle around the y axis in radians.
		* @param z Angle around the z axis in radians.
		*/
		static matrix3 create_rotation(float x, float y, float z);

		/**
		* Returns a matrix with a specified scale.
		*
		* @param scale Scale in the x, y, and z dimensions.
		*/
		static matrix3 create_scale(float scale);

		/**
		* Returns a matrix with a specified scale.
		*
		* @param scale Scale in the x, y, and z dimensions as a vector3.
		*/
		static matrix3 create_scale(const vector3& scale);

		/**
		* Returns a matrix with a specified scale.
		*
		* @param x Scale in the x dimension.
		* @param y Scale in the y dimension.
		* @param z Scale in the z dimension.
		*/
		static matrix3 create_scale(float x, float y, float z);

		friend IWMATH_API std::ostream& operator<<(std::ostream& stream, const matrix3&matrix);
	};
}