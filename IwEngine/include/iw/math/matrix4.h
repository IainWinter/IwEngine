#pragma once

#include "iwmath.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"
#include "matrix3.h"
#include <ostream>

namespace iw {
namespace math {
	/**
	* Represents a 4x4 matrix.
	*/
	struct REFLECT IWMATH_API matrix4 {
		/**
		* 4x4 Identity matrix
		*/
		static const matrix4 identity;

		union {
			REFLECT float elements[4 * 4];
			vector4 rows[4];
		};

		matrix4();

		matrix4(
			float diagonal);

		matrix4(
			float* elements);

		matrix4(
			const matrix3& rot_and_scale);

		matrix4(
			vector3 row0,
			vector3 row1,
			vector3 row2);

		matrix4(
			vector4 row0,
			vector4 row1,
			vector4 row2,
			vector4 row3);

		matrix4(
			float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22
		);

		matrix4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
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
		matrix4 transposed() const;

		/**
		* Returns the inverted version of the matrix.
		*/
		matrix4 inverted() const;

		/**
		* Returns the normalized version of the matrix.
		*/
		matrix4 normalized() const;

		/**
		* Returns a version of the matrix with no rotation.
		*/
		matrix4 cleared_rotation() const;

		/**
		* Returns a version of the matrix with no scale.
		*/
		matrix4 cleared_scale() const;

		/**
		* Returns a version of the martix with no translation.
		*/
		matrix4 cleared_translation() const;

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
		* Clears the translation from the matrix.
		*/
		void clear_translation();

		/**
		* Returns the top left matrix3.
		*/
		matrix3 rotation_and_scale() const;

		/**
		* Returns the rotation of the matrix.
		*/
		quaternion rotation() const;

		/**
		* Returns the axis and angle of rotation.
		*	X, Y, Z is the axis, and W is the angle.
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

		/**
		* Returns the translation of the matrix.
		*/
		vector3 translation() const;

		/**
		* Rotates the matrix to face a specified point in worldspace.
		*/
		void look_at(
			const vector3& eye,
			const vector3& target,
			const vector3& up = vector3::unit_y);

		matrix4 operator+ (
			const matrix4& other) const;

		matrix4 operator- (
			const matrix4& other) const;

		matrix4 operator* (
			const matrix4& other) const;

		matrix4 operator+=(
			const matrix4& other);

		matrix4 operator-=(
			const matrix4& other);

		matrix4 operator*=(
			const matrix4& other);

		matrix4 operator+ (
			float other) const;

		matrix4 operator- (float other) const;

		matrix4 operator* (
			float other) const;

		matrix4 operator/ (
			float other) const;

		matrix4 operator+=(
			float other);

		matrix4 operator-=(
			float other);

		matrix4 operator*=(
			float other);

		matrix4 operator/=(
			float other);

		matrix4 operator-() const;

		float& operator[](
			int index);

		float& operator()(
			int row,
			int col);

		const float& operator()(
			int row,
			int col) const;

		bool operator==(
			const matrix4& other) const;

		bool operator!=(
			const matrix4& other) const;

		/**
		* Returns a matrix with a specified rotation around an axis.
		*
		* @param axisAngle Axis and angle of rotation in one vector.
		*/
		static matrix4 from_axis_angle(
			const vector4& axis_angle);

		/**
		* Returns a matrix with a specified rotation around an axis.
		*
		* @param axis Axis vector
		* @param angle Angle of rotation around the axis in radians.
		*/
		static matrix4 from_axis_angle(
			const vector3& axis,
			float angle);

		/**
		* Returns a matrix with a specified rotation around an axis.
		*
		* @param x X component of the axis vector.
		* @param y Y component of the axis vector.
		* @param z X component of the axis vector.
		* @param angle Angle of rotation around the axis in radians.
		*/
		static matrix4 from_axis_angle(
			float x,
			float y,
			float z,
			float angle);

		/**
		* Returns a matrix with a specified rotation.
		*
		* @param quaternion Quaternion to create matrix from.
		*/
		static matrix4 create_from_quaternion(
			const quaternion& quaternion);

		/**
		* Returns a matrix with a specified rotation around the x axis.
		*
		* @param angle Angle around the x axis in radians.
		*/
		static matrix4 create_rotation_x(
			float angle);

		/**
		* Returns a matrix with a specified rotation around the y axis.
		*
		* @param angle Angle around the y axis in radians.
		*/
		static matrix4 create_rotation_y(
			float angle);

		/**
		* Returns a matrix with a specified rotation around the z axis.
		*
		* @param angle Angle around the z axis in radians.
		*/
		static matrix4 create_rotation_z(
			float angle);

		/**
		* Returns a matrix with a specified rotation.
		*
		* @param angles Euler angles in radians.
		*/
		static matrix4 create_rotation(
			const vector3& angles);

		/**
		* Returns a matrix with a specified rotation.
		*
		* @param x Angle around the x axis in radians.
		* @param y Angle around the y axis in radians.
		* @param z Angle around the z axis in radians.
		*/
		static matrix4 create_rotation(
			float x,
			float y,
			float z);

		/**
		* Returns a matrix with a specified scale.
		*
		* @param scale Scale in the x, y, and z dimensions.
		*/
		static matrix4 create_scale(
			float scale);

		/**
		* Returns a matrix with a specified scale.
		*
		* @param scale Scale in the x, y, and z dimensions as a vector3.
		*/
		static matrix4 create_scale(
			const vector3& scale);

		/**
		* Returns a matrix with a specified scale.
		*
		* @param x Scale in the x dimension.
		* @param y Scale in the y dimension.
		* @param z Scale in the z dimension.
		*/
		static matrix4 create_scale(
			float x,
			float y,
			float z);

		/**
		* Returns a matrix with a specified translation.
		*
		* @param translation Translation along the x, y, and z axes as a vector3.
		*/
		static matrix4 create_translation(
			const vector3& translation);

		/**
		* Returns a matrix with a specified translation.
		*
		* @param x Translation along the in the x axis.
		* @param y Translation along the in the y axis.
		* @param z Translation along the in the z axis.
		*/
		static matrix4 create_translation(
			float x,
			float y,
			float z);

		/**
		* Returns a matrix with an orthographic view.
		*
		* @param width Width of the viewing volume.
		* @param height Height of the viewing volume.
		* @param zNear Min clipping distance of the viewing volume.
		* @param zFar Max clipping distance of the viewing volume.
		*/
		static matrix4 create_orthographic(
			float width,
			float height,
			float zNear,
			float zFar);

		/**
		* Returns a matrix with an orthographic view.
		*
		* @param left Left side of the viewing volume.
		* @param right Right side of the viewing volume.
		* @param bottom Bottom side of the viewing volume.
		* @param Top Top side of the viewing volume.
		* @param zNear Min clipping distance of the viewing volume.
		* @param zFar Max clipping distance of the viewing volume.
		*/
		static matrix4 create_orthographic_off_center(
			float left,
			float right,
			float bottom,
			float top,
			float zNear,
			float zFar);

		/**
		* Returns a matrix with an perspective view.
		*
		* @param fov Angle of the field of view in radians.
		* @param aspect Aspect ratio of the viewing volume.
		* @param zNear Min clipping distance of the viewing volume.
		* @param zFar Max clipping distance of the viewing volume.
		*/
		static matrix4 create_perspective_field_of_view(
			float fov,
			float aspect,
			float zNear,
			float zFar);

		/**
		* Returns a matrix with an perspective view.
		*
		* @param left Left side of the viewing volume.
		* @param right Right side of the viewing volume.
		* @param bottom Bottom side of the viewing volume.
		* @param Top Top side of the viewing volume.
		* @param zNear Min clipping distance of the viewing volume.
		* @param zFar Max clipping distance of the viewing volume.
		*/
		static matrix4 create_perspective_off_center(
			float left,
			float right,
			float bottom,
			float top,
			float zNear,
			float zFar);

		/**
		* Returns a matrix that is rotated to face a target
		*	in space relitive to 'eye' and 'up'.
		*
		* @param eye Position of eye.
		* @param target Position to point towards.
		* @param up Vector of the upward direction.
		*/
		static matrix4 create_look_at(
			const vector3& eye,
			const vector3& target,
			const vector3& up = vector3::unit_y);

		IWMATH_API
		friend std::ostream& operator<<(
			std::ostream& stream,
			const matrix4& a);
	};
}

	using namespace math;
}
