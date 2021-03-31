#include "iw/math/matrix4.h"

namespace iw {
	const matrix4 matrix4::identity = matrix4(1.0f);

	matrix4::matrix4() {
		memset(elements, 0, 4 * 4 * sizeof(float));
	}

	matrix4::matrix4(
		float diagonal)
	{
		memset(elements, 0, 4 * 4 * sizeof(float));
		elements[0 + 0 * 4] = diagonal;
		elements[1 + 1 * 4] = diagonal;
		elements[2 + 2 * 4] = diagonal;
		elements[3 + 3 * 4] = diagonal;
	}

	matrix4::matrix4(
		float* elements)
	{
		memcpy(this->elements, elements, 4 * 4 * sizeof(float));
	}

	matrix4::matrix4(
		const matrix3& rot_and_scale)
	{
		rows[0] = rot_and_scale.rows[0];
		rows[1] = rot_and_scale.rows[1];
		rows[2] = rot_and_scale.rows[2];
		rows[3] = vector4::unit_w;
	}

	matrix4::matrix4(
		vector3 row0,
		vector3 row1,
		vector3 row2)
	{
		rows[0] = row0;
		rows[1] = row1;
		rows[2] = row2;
		rows[3] = vector4::unit_w;
	}

	matrix4::matrix4(
		vector4 row0,
		vector4 row1,
		vector4 row2,
		vector4 row3)
	{
		rows[0] = row0;
		rows[1] = row1;
		rows[2] = row2;
		rows[3] = row3;
	}

	matrix4::matrix4(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22)
	{
		elements[0]  = m00; elements[1]  = m01;
		elements[2]  = m02; elements[3]  = 0;
		elements[4]  = m10; elements[5]  = m11;
		elements[6]  = m12; elements[7]  = 0;
		elements[8]  = m20; elements[9]  = m21;
		elements[10] = m22; elements[11] = 0;
		elements[12] = 0;   elements[13] = 0;
		elements[14] = 0;   elements[15] = 1;
	}

	matrix4::matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		elements[0]  = m00; elements[1]  = m01;
		elements[2]  = m02; elements[3]  = m03;
		elements[4]  = m10; elements[5]  = m11;
		elements[6]  = m12; elements[7]  = m13;
		elements[8]  = m20; elements[9]  = m21;
		elements[10] = m22; elements[11] = m23;
		elements[12] = m30; elements[13] = m31;
		elements[14] = m32; elements[15] = m33;
	}

	float matrix4::determinant() const {
		return rows[0].x * rows[1].y * rows[2].z * rows[3].z
			 - rows[0].x * rows[1].y * rows[2].w * rows[3].z
			 + rows[0].x * rows[1].z * rows[2].w * rows[3].y
			 - rows[0].x * rows[1].z * rows[2].y * rows[3].z
			 + rows[0].x * rows[1].w * rows[2].y * rows[3].z
			 - rows[0].x * rows[1].w * rows[2].z * rows[3].y
			 - rows[0].y * rows[1].z * rows[2].w * rows[3].x
			 + rows[0].y * rows[1].z * rows[2].x * rows[3].z
			 - rows[0].y * rows[1].w * rows[2].x * rows[3].z
			 + rows[0].y * rows[1].w * rows[2].z * rows[3].x
			 - rows[0].y * rows[1].x * rows[2].z * rows[3].z
			 + rows[0].y * rows[1].x * rows[2].w * rows[3].z
			 + rows[0].z * rows[1].w * rows[2].x * rows[3].y
			 - rows[0].z * rows[1].w * rows[2].y * rows[3].x
			 + rows[0].z * rows[1].x * rows[2].y * rows[3].z
			 - rows[0].z * rows[1].x * rows[2].w * rows[3].y
			 + rows[0].z * rows[1].y * rows[2].w * rows[3].x
			 - rows[0].z * rows[1].y * rows[2].x * rows[3].z
			 - rows[0].w * rows[1].x * rows[2].y * rows[3].z
			 + rows[0].w * rows[1].x * rows[2].z * rows[3].y
			 - rows[0].w * rows[1].y * rows[2].z * rows[3].x
			 + rows[0].w * rows[1].y * rows[2].x * rows[3].z
			 - rows[0].w * rows[1].z * rows[2].x * rows[3].y
			 + rows[0].w * rows[1].z * rows[2].y * rows[3].x;
	}

	float matrix4::trace() const {
		return rows[0].x + rows[1].y + rows[2].z + rows[3].w;
	}

	matrix4 matrix4::transposed() const {
		matrix4 tmp = *this;
		tmp.transpose();
		return tmp;
	}

	matrix4 matrix4::inverted() const {
		matrix4 tmp = *this;
		tmp.invert();
		return tmp;
	}

	matrix4 matrix4::normalized() const {
		matrix4 tmp = *this;
		tmp.normalize();
		return tmp;
	}

	matrix4 matrix4::cleared_rotation() const {
		matrix4 tmp = *this;
		tmp.clear_rotation();
		return tmp;
	}

	matrix4 matrix4::cleared_scale() const {
		matrix4 tmp = *this;
		tmp.clear_scale();
		return tmp;
	}

	matrix4 matrix4::cleared_translation() const {
		matrix4 tmp = *this;
		tmp.clear_translation();
		return tmp;
	}

	void matrix4::transpose() {
		matrix4 tmp = *this;
		rows[0].x = tmp.rows[0].x;
		rows[0].y = tmp.rows[1].x;
		rows[0].z = tmp.rows[2].x;
		rows[0].w = tmp.rows[3].x;
		rows[1].x = tmp.rows[0].y;
		rows[1].y = tmp.rows[1].y;
		rows[1].z = tmp.rows[2].y;
		rows[1].w = tmp.rows[3].y;
		rows[2].x = tmp.rows[0].z;
		rows[2].y = tmp.rows[1].z;
		rows[2].z = tmp.rows[2].z;
		rows[2].w = tmp.rows[3].z;
		rows[3].x = tmp.rows[0].w;
		rows[3].y = tmp.rows[1].w;
		rows[3].z = tmp.rows[2].w;
		rows[3].w = tmp.rows[3].w;
	}

	void matrix4::invert() {
		float m00 = elements[0];
		float m01 = elements[1];
		float m02 = elements[2];
		float m03 = elements[3];
		float m10 = elements[4];
		float m11 = elements[5];
		float m12 = elements[6];
		float m13 = elements[7];
		float m20 = elements[8];
		float m21 = elements[9];
		float m22 = elements[10];
		float m23 = elements[11];
		float m30 = elements[12];
		float m31 = elements[13];
		float m32 = elements[14];
		float m33 = elements[15];

		matrix4 inv = matrix4(
			 m11 * m22 * m33 - m11 * m23 * m32 - m21 * m12 * m33 + m21 * m13 * m32 + m31 * m12 * m23 - m31 * m13 * m22,
			-m10 * m22 * m33 + m10 * m23 * m32 + m20 * m12 * m33 - m20 * m13 * m32 - m30 * m12 * m23 + m30 * m13 * m22,
			 m10 * m21 * m33 - m10 * m23 * m31 - m20 * m11 * m33 + m20 * m13 * m31 + m30 * m11 * m23 - m30 * m13 * m21,
			-m10 * m21 * m32 + m10 * m22 * m31 + m20 * m11 * m32 - m20 * m12 * m31 - m30 * m11 * m22 + m30 * m12 * m21,
			-m01 * m22 * m33 + m01 * m23 * m32 + m21 * m02 * m33 - m21 * m03 * m32 - m31 * m02 * m23 + m31 * m03 * m22,
			 m00 * m22 * m33 - m00 * m23 * m32 - m20 * m02 * m33 + m20 * m03 * m32 + m30 * m02 * m23 - m30 * m03 * m22,
			-m00 * m21 * m33 + m00 * m23 * m31 + m20 * m01 * m33 - m20 * m03 * m31 - m30 * m01 * m23 + m30 * m03 * m21,
			 m00 * m21 * m32 - m00 * m22 * m31 - m20 * m01 * m32 + m20 * m02 * m31 + m30 * m01 * m22 - m30 * m02 * m21,
			 m01 * m12 * m33 - m01 * m13 * m32 - m11 * m02 * m33 + m11 * m03 * m32 + m31 * m02 * m13 - m31 * m03 * m12,
			-m00 * m12 * m33 + m00 * m13 * m32 + m10 * m02 * m33 - m10 * m03 * m32 - m30 * m02 * m13 + m30 * m03 * m12,
			 m00 * m11 * m33 - m00 * m13 * m31 - m10 * m01 * m33 + m10 * m03 * m31 + m30 * m01 * m13 - m30 * m03 * m11,
			-m00 * m11 * m32 + m00 * m12 * m31 + m10 * m01 * m32 - m10 * m02 * m31 - m30 * m01 * m12 + m30 * m02 * m11,
			-m01 * m12 * m23 + m01 * m13 * m22 + m11 * m02 * m23 - m11 * m03 * m22 - m21 * m02 * m13 + m21 * m03 * m12,
			 m00 * m12 * m23 - m00 * m13 * m22 - m10 * m02 * m23 + m10 * m03 * m22 + m20 * m02 * m13 - m20 * m03 * m12,
			-m00 * m11 * m23 + m00 * m13 * m21 + m10 * m01 * m23 - m10 * m03 * m21 - m20 * m01 * m13 + m20 * m03 * m11,
			 m00 * m11 * m22 - m00 * m12 * m21 - m10 * m01 * m22 + m10 * m02 * m21 + m20 * m01 * m12 - m20 * m02 * m11
		);

		//matrix4 inv = matrix4(
		//	 (m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31)),
		//	-(m01 * (m22 * m33 - m23 * m32) - m02 * (m21 * m33 - m23 * m31) + m03 * (m21 * m32 - m22 * m31)),
		//	 (m01 * (m12 * m33 - m13 * m32) - m02 * (m11 * m33 - m13 * m31) + m03 * (m11 * m32 - m12 * m31)),
		//	-(m01 * (m12 * m23 - m13 * m22) - m02 * (m11 * m23 - m13 * m21) + m03 * (m11 * m22 - m12 * m21)),
		//	-(m10 * (m22 * m33 - m23 * m32) - m12 * (m20 * m33 - m23 * m30) + m13 * (m20 * m32 - m22 * m30)),
		//	 (m00 * (m22 * m33 - m23 * m32) - m02 * (m20 * m33 - m23 * m30) + m03 * (m20 * m32 - m22 * m30)),
		//	-(m00 * (m12 * m33 - m13 * m32) - m02 * (m10 * m33 - m13 * m30) + m03 * (m10 * m32 - m12 * m30)),
		//	 (m00 * (m12 * m23 - m13 * m22) - m02 * (m10 * m23 - m13 * m20) + m03 * (m10 * m22 - m12 * m20)),
		//	 (m10 * (m21 * m33 - m23 * m31) - m11 * (m20 * m33 - m23 * m30) + m13 * (m20 * m31 - m21 * m30)),
		//	-(m00 * (m21 * m33 - m23 * m31) - m01 * (m20 * m33 - m23 * m30) + m03 * (m20 * m31 - m21 * m30)),
		//	 (m00 * (m11 * m33 - m13 * m31) - m01 * (m10 * m33 - m13 * m30) + m03 * (m10 * m31 - m11 * m30)),
		//	-(m00 * (m11 * m23 - m13 * m21) - m01 * (m10 * m23 - m13 * m20) + m03 * (m10 * m21 - m11 * m20)),
		//	-(m10 * (m21 * m32 - m22 * m31) - m11 * (m20 * m32 - m22 * m30) + m12 * (m20 * m31 - m21 * m30)),
		//	 (m00 * (m21 * m32 - m22 * m31) - m01 * (m20 * m32 - m22 * m30) + m02 * (m20 * m31 - m21 * m30)),
		//	-(m00 * (m11 * m32 - m12 * m31) - m01 * (m10 * m32 - m12 * m30) + m02 * (m10 * m31 - m11 * m30)),
		//	 (m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) + m02 * (m10 * m21 - m11 * m20))
		//);

		float det = m00 * inv[0] + m01 * inv[4] + m02 * inv[8] + m03 * inv[12];

		if (det == 0.0f) {
			throw std::invalid_argument("Determinant is zero, "
				"therefore inverse matrix doesn't exist.");
		}

		float invDet = 1.0f / det;

		for (int i = 0; i < 16; i++) {
			inv[i] *= invDet;
		}

		*this = inv;
	}

	void matrix4::normalize() {
		float det = determinant();
		if (almost_equal(det, 0.0f, 6)) {
			return;
		}

		*this /= det;
	}

	void matrix4::clear_rotation() {
		rows[0] = vector4(rows[0].length(), 0, 0, 0);
		rows[1] = vector4(0, rows[1].length(), 0, 0);
		rows[2] = vector4(0, 0, rows[2].length(), 0);
	}

	void matrix4::clear_scale() {
		rows[0].normalize();
		rows[1].normalize();
		rows[2].normalize();
	}

	void matrix4::clear_translation() {
		rows[3] = vector4(0, 0, 0, 1);
	}

	matrix3 matrix4::rotation_and_scale() const {
		return matrix3(rows[0].xyz(), rows[1].xyz(), rows[2].xyz());
	}

	quaternion matrix4::rotation() const {
		return rotation_and_scale().rotation();
	}

	vector4 matrix4::axis_angle() const {
		return rotation_and_scale().axis_angle();
	}

	vector3 matrix4::euler_angles() const {
		return rotation_and_scale().euler_angles();
	}

	vector3 matrix4::scale() const {
		return rotation_and_scale().scale();
	}

	vector3 matrix4::translation() const {
		return rows[3].xyz();
	}

	void matrix4::look_at(
		const vector3& eye,
		const vector3& target,
		const vector3& up)
	{
		vector3 z = (eye - target).normalized();
		vector3 x = up.cross(z).normalized();
		vector3 y = z.cross(x).normalized();

		rows[0] = vector4(x.x, y.x, z.x, 0);
		rows[1] = vector4(x.y, y.y, z.y, 0);
		rows[2] = vector4(x.z, y.z, z.z, 0);
		rows[3] = vector4(
			-((x.x * eye.x) + (x.y * eye.y) + (x.z * eye.z)),
			-((y.x * eye.x) + (y.y * eye.y) + (y.z * eye.z)),
			-((z.x * eye.x) + (z.y * eye.y) + (z.z * eye.z)),
			1);
	}

	float& matrix4::operator[](
		int index)
	{
		return elements[index];
	}

	float& matrix4::operator()(
		int row,
		int col)
	{
		if (row > 3 || col > 3 || 0 > row || 0 > col) {
			throw std::out_of_range("Row/Col is outside the "
				"bounds of this matrix.");
		}

		return elements[col + row * 4];
	}

	const float& matrix4::operator()(
		int row, 
		int col) const
	{
		if (row > 3 || col > 3 || 0 > row || 0 > col) {
			throw std::out_of_range("Row/Col is outside the "
				"bounds of this matrix.");
		}

		return elements[col + row * 4];
	}

	matrix4 matrix4::operator+(
		const matrix4& other) const
	{
		return matrix4(
			rows[0] + other.rows[0],
			rows[1] + other.rows[1],
			rows[2] + other.rows[2],
			rows[3] + other.rows[3]
		);
	}

	matrix4 matrix4::operator-(
		const matrix4& other) const
	{
		return matrix4(
			rows[0] - other.rows[0],
			rows[1] - other.rows[1],
			rows[2] - other.rows[2],
			rows[3] - other.rows[3]
		);
	}

	matrix4 matrix4::operator*(
		const matrix4& other) const
	{
		float 
			lM11 = rows[0].x, lM12 = rows[0].y,
			lM13 = rows[0].z, lM14 = rows[0].w,
			lM21 = rows[1].x, lM22 = rows[1].y,
			lM23 = rows[1].z, lM24 = rows[1].w,
			lM31 = rows[2].x, lM32 = rows[2].y,
			lM33 = rows[2].z, lM34 = rows[2].w,
			lM41 = rows[3].x, lM42 = rows[3].y,
			lM43 = rows[3].z, lM44 = rows[3].w,

			rM11 = other.rows[0].x, rM12 = other.rows[0].y,
			rM13 = other.rows[0].z, rM14 = other.rows[0].w,
			rM21 = other.rows[1].x, rM22 = other.rows[1].y,
			rM23 = other.rows[1].z, rM24 = other.rows[1].w,
			rM31 = other.rows[2].x, rM32 = other.rows[2].y,
			rM33 = other.rows[2].z, rM34 = other.rows[2].w,
			rM41 = other.rows[3].x, rM42 = other.rows[3].y,
			rM43 = other.rows[3].z, rM44 = other.rows[3].w;

		return matrix4(
			(((lM11 * rM11) + (lM12 * rM21)) + (lM13 * rM31)) + (lM14 * rM41),
			(((lM11 * rM12) + (lM12 * rM22)) + (lM13 * rM32)) + (lM14 * rM42),
			(((lM11 * rM13) + (lM12 * rM23)) + (lM13 * rM33)) + (lM14 * rM43),
			(((lM11 * rM14) + (lM12 * rM24)) + (lM13 * rM34)) + (lM14 * rM44),
			(((lM21 * rM11) + (lM22 * rM21)) + (lM23 * rM31)) + (lM24 * rM41),
			(((lM21 * rM12) + (lM22 * rM22)) + (lM23 * rM32)) + (lM24 * rM42),
			(((lM21 * rM13) + (lM22 * rM23)) + (lM23 * rM33)) + (lM24 * rM43),
			(((lM21 * rM14) + (lM22 * rM24)) + (lM23 * rM34)) + (lM24 * rM44),
			(((lM31 * rM11) + (lM32 * rM21)) + (lM33 * rM31)) + (lM34 * rM41),
			(((lM31 * rM12) + (lM32 * rM22)) + (lM33 * rM32)) + (lM34 * rM42),
			(((lM31 * rM13) + (lM32 * rM23)) + (lM33 * rM33)) + (lM34 * rM43),
			(((lM31 * rM14) + (lM32 * rM24)) + (lM33 * rM34)) + (lM34 * rM44),
			(((lM41 * rM11) + (lM42 * rM21)) + (lM43 * rM31)) + (lM44 * rM41),
			(((lM41 * rM12) + (lM42 * rM22)) + (lM43 * rM32)) + (lM44 * rM42),
			(((lM41 * rM13) + (lM42 * rM23)) + (lM43 * rM33)) + (lM44 * rM43),
			(((lM41 * rM14) + (lM42 * rM24)) + (lM43 * rM34)) + (lM44 * rM44));
	}

	matrix4 matrix4::operator+=(
		const matrix4& other)
	{
		return *this = *this + other;
	}

	matrix4 matrix4::operator-=(
		const matrix4& other)
	{
		return *this = *this - other;
	}

	matrix4 matrix4::operator*=(
		const matrix4& other)
	{
		return *this = *this * other;
	}

	matrix4 matrix4::operator+(
		float other) const
	{
		return matrix4(
			rows[0] + other,
			rows[1] + other,
			rows[2] + other,
			rows[3] + other
		);
	}

	matrix4 matrix4::operator-(
		float other) const
	{
		return matrix4(
			rows[0] - other,
			rows[1] - other,
			rows[2] - other,
			rows[3] - other
		);
	}

	matrix4 matrix4::operator*(
		float other) const
	{
		return matrix4(
			rows[0] * other,
			rows[1] * other,
			rows[2] * other,
			rows[3] * other
		);
	}


	matrix4 matrix4::operator/(
		float other) const
	{
		return matrix4(
			rows[0] / other,
			rows[1] / other,
			rows[2] / other,
			rows[3] / other
		);
	}

	matrix4 matrix4::operator+=(
		float other)
	{
		return *this = *this + other;
	}

	matrix4 matrix4::operator-=(
		float other)
	{
		return *this = *this - other;
	}

	matrix4 matrix4::operator*=(
		float other)
	{
		return *this = *this * other;
	}

	matrix4 matrix4::operator/=(
		float other)
	{
		return *this = *this / other;
	}

	matrix4 matrix4::operator-() const
	{
		return matrix4(-rows[0], -rows[1], -rows[2], -rows[3]);
	}

	bool matrix4::operator==(
		const matrix4& other) const
	{
		return rows[0] == other.rows[0]
			&& rows[1] == other.rows[1]
			&& rows[2] == other.rows[2]
			&& rows[3] == other.rows[3];
	}

	bool matrix4::operator!=(
		const matrix4& other) const
	{
		return !operator==(other);
	}

	matrix4 matrix4::from_axis_angle(
		const vector4& axis_angle)
	{
		return from_axis_angle(
			axis_angle.x, axis_angle.y, axis_angle.z, axis_angle.w);
	}

	matrix4 matrix4::from_axis_angle(
		const vector3& axis,
		float angle)
	{
		return from_axis_angle(
			axis.x, axis.y, axis.z, angle);
	}

	matrix4 matrix4::from_axis_angle(
		float x,
		float y,
		float z,
		float angle)
	{
		return matrix3::from_axis_angle(x, y, z, angle);
	}

	matrix4 matrix4::create_from_quaternion(
		const quaternion& quaternion)
	{
		return matrix3::create_from_quaternion(quaternion);
	}

	matrix4 matrix4::create_rotation_x(
		float angle)
	{
		return matrix3::create_rotation_x(angle);
	}

	matrix4 matrix4::create_rotation_y(
		float angle)
	{
		return matrix3::create_rotation_y(angle);

	}

	matrix4 matrix4::create_rotation_z(
		float angle)
	{
		return matrix3::create_rotation_z(angle);

	}

	matrix4 matrix4::create_rotation(
		const vector3& angles)
	{
		return matrix4::create_rotation(angles.x, angles.y, angles.z);
	}

	matrix4 matrix4::create_rotation(
		float x,
		float y,
		float z)
	{
		return matrix3::create_rotation(x, y, z);
	}

	matrix4 matrix4::create_translation(
		const vector3& translation)
	{
		return create_translation(translation.x, translation.y, translation.z);
	}

	matrix4 matrix4::create_scale(
		float scale)
	{
		return create_scale(scale, scale, scale);
	}

	matrix4 matrix4::create_scale(
		const vector3& scale)
	{
		return create_scale(scale.x, scale.y, scale.z);
	}

	matrix4 matrix4::create_scale(
		float x,
		float y,
		float z)
	{
		return matrix3::create_scale(x, y, z);
	}

	matrix4 matrix4::create_translation(
		float x,
		float y,
		float z)
	{
		return matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, z, 1
		);
	}

	matrix4 matrix4::create_orthographic(
		float width,
		float height,
		float zNear,
		float zFar)
	{
		return create_orthographic_off_center(
			-width  / 2,
			width   / 2,
			-height / 2,
			height  / 2,
			zNear,
			zFar);
	}

	matrix4 matrix4::create_orthographic_off_center(
		float left,
		float right,
		float bottom,
		float top,
		float zNear,
		float zFar)
	{
		matrix4 out = matrix4::identity;

		float invRL = 1.0f / (right - left);
		float invTB = 1.0f / (top - bottom);
		float invFN = 1.0f / (zFar - zNear);

		out.rows[0].x = 2 * invRL;
		out.rows[1].y = 2 * invTB;
		out.rows[2].z = -2 * invFN;
		out.rows[3].x = -(right + left) * invRL;
		out.rows[3].y = -(top + bottom) * invTB;
		out.rows[3].z = -(zFar + zNear) * invFN;

		return out;
	}

	matrix4 matrix4::create_perspective_field_of_view(
		float fov,
		float aspect,
		float zNear,
		float zFar)
	{
		if (fov <= 0 || fov > Pi) {
			throw std::invalid_argument("fovy is not betwwen 0 and pi.");
		}

		if (aspect <= 0) {
			throw std::invalid_argument("aspect is not greater than 0.");
		}

		if (zNear <= 0) {
			throw std::invalid_argument("zNear is not greater than 0.");
		}

		if (zFar <= 0) {
			throw std::invalid_argument("zFar is not greater than 0.");
		}

		float yMax = zNear * tanf(0.5f * fov);
		float yMin = -yMax;
		float xMin = yMin * aspect;
		float xMax = yMax * aspect;

		return create_perspective_off_center(
			xMin, xMax, yMin, yMax, zNear, zFar);
	}

	matrix4 matrix4::create_perspective_off_center(
		float left,
		float right,
		float bottom,
		float top,
		float zNear,
		float zFar)
	{
		if (zNear <= 0) {
			throw std::invalid_argument("zNear is not greater than 0.");
		}

		if (zFar <= 0) {
			throw std::invalid_argument("zFar is not greater than 0.");
		}

		float x = 2 * zNear / (right - left);
		float y = 2 * zNear / (top - bottom);
		float a = (right + left) / (right - left);
		float b = (top + bottom) / (top - bottom);
		float c = -(zFar + zNear) / (zFar - zNear);
		float d = -2 * zFar * zNear / (zFar - zNear);

		return matrix4(
			x, 0, 0, 0,
			0, y, 0, 0,
			a, b, c, -1, // <--- is this wrong????
			0, 0, d, 0);

		//float x = 2.0f * zNear / (right - left);
		//float y = 2 * zNear / (top - bottom);
		//float a = -(zFar + zNear) / (zFar - zNear);
		//float b = -zNear * (right + left) / (right - left);
		//float c = -zNear * (top + bottom) / (top - bottom);
		//float d = 2 * zFar * zNear / (zNear - zFar);

		//return matrix4(
		//	x, 0, 0, 0,
		//	0, y, 0, 0,
		//	0, 0, a, -1,
		//	b, c, d, 0
		//);
	}

	matrix4 matrix4::create_look_at(
		const vector3& eye,
		const vector3& target,
		const vector3& up)
	{
		matrix4 mat = matrix4();
		mat.look_at(eye, target, up);
		return mat;
	}

	std::ostream& math::operator<<(
		std::ostream& ostream,
		const matrix4& a)
	{
		return ostream
			<< a.rows[0] << std::endl
			<< a.rows[1] << std::endl
			<< a.rows[2] << std::endl
			<< a.rows[3];
	}
}
