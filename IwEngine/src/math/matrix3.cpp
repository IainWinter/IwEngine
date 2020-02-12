#include "iw/math/matrix3.h"
#include "iw/math/matrix2.h"

namespace iw {
	const matrix3 matrix3::identity = matrix3(1.0f);

	matrix3::matrix3() {
		memset(elements, 0, 3 * 3 * sizeof(float));
	}

	matrix3::matrix3(
		float diagonal)
	{
		memset(elements, 0, 3 * 3 * sizeof(float));
		elements[0 + 0 * 3] = diagonal;
		elements[1 + 1 * 3] = diagonal;
		elements[2 + 2 * 3] = diagonal;
	}

	matrix3::matrix3(
		float* elements)
	{
		memcpy(this->elements, elements, 3 * 3 * sizeof(float));
	}

	matrix3::matrix3(
		vector3 row0,
		vector3 row1,
		vector3 row2)
	{
		rows[0] = row0;
		rows[1] = row1;
		rows[2] = row2;
	}

	matrix3::matrix3(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22)
	{
		elements[0] = m00; elements[1] = m01; elements[2] = m02;
		elements[3] = m10; elements[4] = m11; elements[5] = m12;
		elements[6] = m20; elements[7] = m21; elements[8] = m22;
	}

	float matrix3::determinant() const {
		return rows[0].x * (rows[1].y * rows[2].z - rows[1].z * rows[2].y)
			 - rows[0].y * (rows[1].x * rows[2].z - rows[1].z * rows[2].x)
			 + rows[0].z * (rows[1].x * rows[2].y - rows[1].y * rows[2].x);
	}

	float matrix3::trace() const {
		return rows[0].x + rows[1].y + rows[2].z;
	}

	matrix3 matrix3::transposed() const {
		matrix3 tmp = *this;
		tmp.transpose();
		return tmp;
	}

	matrix3 matrix3::inverted() const {
		matrix3 tmp = *this;
		tmp.invert();
		return tmp;
	}

	matrix3 matrix3::normalized() const {
		matrix3 tmp = *this;
		tmp.normalize();
		return tmp;
	}

	matrix3 matrix3::cleared_rotation() const {
		matrix3 tmp = *this;
		tmp.clear_rotation();
		return tmp;
	}

	matrix3 matrix3::cleared_scale() const {
		matrix3 tmp = *this;
		tmp.clear_scale();
		return tmp;
	}

	void matrix3::transpose() {
		matrix3 tmp = *this;
		rows[0].x = tmp.rows[0].x;
		rows[0].y = tmp.rows[1].x;
		rows[0].z = tmp.rows[2].x;
		rows[1].x = tmp.rows[0].y;
		rows[1].y = tmp.rows[1].y;
		rows[1].z = tmp.rows[2].y;
		rows[2].x = tmp.rows[0].z;
		rows[2].y = tmp.rows[1].z;
		rows[2].z = tmp.rows[2].z;
	}

	void matrix3::invert() {
		float det = determinant();
		if (almost_equal(det, 0, 6)) {
			throw std::invalid_argument("Determinant is zero, "
				"therefore inverse matrix doesn't exist.");
		}

		float invDet = 1 / det;

		*this = matrix3(
			invDet *  (elements[4] * elements[8] - elements[5] * elements[7]),
			invDet * -(elements[1] * elements[8] - elements[2] * elements[7]),
			invDet *  (elements[1] * elements[5] - elements[2] * elements[4]),
			invDet * -(elements[3] * elements[8] - elements[5] * elements[6]),
			invDet *  (elements[0] * elements[8] - elements[2] * elements[6]),
			invDet * -(elements[0] * elements[5] - elements[2] * elements[3]),
			invDet *  (elements[3] * elements[7] - elements[4] * elements[6]),
			invDet * -(elements[0] * elements[7] - elements[1] * elements[6]),
			invDet *  (elements[0] * elements[4] - elements[1] * elements[3])
		);
	}

	void matrix3::normalize() {
		float det = determinant();
		if (almost_equal(det, 0, 6)) {
			return;
		}

		*this /= det;
	}

	void matrix3::clear_rotation() {
		rows[0] = vector3(rows[0].length(), 0, 0);
		rows[1] = vector3(0, rows[1].length(), 0);
		rows[2] = vector3(0, 0, rows[2].length());
	}

	void matrix3::clear_scale() {
		vector3 c1 = vector3(elements[0], elements[3], elements[6]);
		vector3 c2 = vector3(elements[1], elements[4], elements[7]);
		vector3 c3 = vector3(elements[2], elements[5], elements[8]);

		c1.normalize();
		c2.normalize();
		c3.normalize();

		rows[0] = vector3(c1.x, c2.x, c3.x);
		rows[1] = vector3(c1.y, c2.y, c3.y);
		rows[2] = vector3(c1.z, c2.z, c3.z);
	}

	quaternion matrix3::rotation() const {
		matrix3 rot = cleared_scale();
		float tr = rot.trace();

		quaternion q = quaternion::identity;

		if (tr > 0) {
			//scale = 4*qw
			float scale = sqrt(tr + 1) * 2;
			q.w = scale / 4;
			q.x = (rot(2, 1) - rot(1, 2)) / scale;
			q.y = (rot(0, 2) - rot(2, 0)) / scale;
			q.z = (rot(1, 0) - rot(0, 1)) / scale;
		}

		else if (rot(0, 0) > rot(1, 1) && rot(0, 0) > rot(2, 2)) {
			//scale = 4*qx
			float scale = sqrt(1 + rot(0, 0) - rot(1, 1) - rot(2, 2)) * 2;
			q.w = (rot(2, 1) - rot(1, 2)) / scale;
			q.x = scale / 4;
			q.y = (rot(0, 1) + rot(1, 0)) / scale;
			q.z = (rot(0, 2) + rot(2, 0)) / scale;
		}

		else if (rot(1, 1) > rot(2, 2)) {
			//scale = 4*qy
			float scale = sqrt(1 + rot(1, 1) - rot(0, 0) - rot(2, 2)) * 2;
			q.w = (rot(0, 2) - rot(2, 0)) / scale;
			q.x = (rot(0, 1) + rot(1, 0)) / scale;
			q.y = scale / 4;
			q.z = (rot(1, 2) + rot(2, 1)) / scale;
		}

		else {
			//scale = 4*qz
			float scale = sqrt(1 + rot(2, 2) - rot(0, 0) - rot(1, 1)) * 2;
			q.w = (rot(1, 0) - rot(0, 1)) / scale;
			q.x = (rot(0, 2) + rot(2, 0)) / scale;
			q.y = (rot(1, 2) + rot(2, 1)) / scale;
			q.z = scale / 4;
		}

		return q.normalized();
	}

	vector4 matrix3::axis_angle() const {
		matrix3 rot = cleared_scale();

		vector3 v = vector3::zero;

		float angle = acosf((rot.trace() - 1) / 2);
		float scale = 1 / (2 * sinf(angle));

		if (0 < angle && angle < Pi) {
			v.x = rot(2, 1) - rot(1, 2);
			v.y = rot(0, 2) - rot(2, 0);
			v.z = rot(1, 0) - rot(0, 1);
		}

		else if (almost_equal(angle, Pi, 6)) {
			float rot00 = rot(0, 0);
			float rot11 = rot(1, 1);
			float rot22 = rot(2, 2);

			if (rot00 > rot11 && rot00 > rot22) {
				v.x = rot00 + 1;
				v.y = rot(0, 1);
				v.z = rot(0, 2);
			}

			else if (rot11 > rot00 && rot11 > rot22) {
				v.x = rot(1, 0);
				v.y = rot11 + 1;
				v.z = rot(1, 2);
			}

			else {
				v.x = rot(2, 0);
				v.y = rot(1, 1);
				v.z = rot22 + 1;
			}
		}

		v.x *= scale;
		v.y *= scale;
		v.z *= scale;

		//Mirror axis and angle if axis is negitive
		//TODO: I don't think this is needed
		//if (0 >= v.x && 0 >= v.y && 0 >= v.z) {
		//	v = -v;
		//	angle = -angle + Pi2;
		//}

		return vector4(v, angle);
	}

	vector3 matrix3::euler_angles() const {
		matrix3 rot = cleared_scale();

		vector3 v = vector3::zero;

		float rot02 = rot(0, 2);

		if (rot02 == -1) {
			v.y = Pi / 2;
			v.x = atan2(rot(1, 0), rot(2, 0));
		}

		else if (rot02 == 1) {
			v.y = -Pi / 2;
			v.x = atan2(-rot(1, 0), -rot(2, 0));
		}

		else {
			v.y = -asinf(rot02);
			v.x = atan2f(rot(1, 2) / cosf(v.y), rot(2, 2) / cosf(v.y));
			v.z = atan2f(rot(0, 1) / cosf(v.y), rot(0, 0) / cosf(v.y));
		}

		return v;
	}

	vector3 matrix3::scale() const {
		return vector3(rows[0].length(), rows[1].length(), rows[2].length());
	}

	matrix3 matrix3::operator+(
		const matrix3& other) const
	{
		return matrix3(
			rows[0] + other.rows[0],
			rows[1] + other.rows[1],
			rows[2] + other.rows[2]
		);
	}

	matrix3 matrix3::operator-(
		const matrix3& other) const
	{
		return matrix3(
			rows[0] - other.rows[0],
			rows[1] - other.rows[1],
			rows[2] - other.rows[2]
		);
	}

	matrix3 matrix3::operator*(
		const matrix3& other) const
	{
		float a00 = rows[0].x;
		float a10 = rows[0].y;
		float a20 = rows[0].z;
		float a01 = rows[1].x;
		float a11 = rows[1].y;
		float a21 = rows[1].z;
		float a02 = rows[2].x;
		float a12 = rows[2].y;
		float a22 = rows[2].z;

		float b00 = other.rows[0].x;
		float b10 = other.rows[0].y;
		float b20 = other.rows[0].z;
		float b01 = other.rows[1].x;
		float b11 = other.rows[1].y;
		float b21 = other.rows[1].z;
		float b02 = other.rows[2].x;
		float b12 = other.rows[2].y;
		float b22 = other.rows[2].z;

		return matrix3(
			a00 * b00 + a10 * b01 + a20 * b02,
			a00 * b10 + a10 * b11 + a20 * b12,
			a00 * b20 + a10 * b21 + a20 * b22,
			a01 * b00 + a11 * b01 + a21 * b02,
			a01 * b10 + a11 * b11 + a21 * b12,
			a01 * b20 + a11 * b21 + a21 * b22,
			a02 * b00 + a12 * b01 + a22 * b02,
			a02 * b10 + a12 * b11 + a22 * b12,
			a02 * b20 + a12 * b21 + a22 * b22);
	}

	matrix3 matrix3::operator+=(
		const matrix3& other)
	{
		return *this = (*this) + other;
	}

	matrix3 matrix3::operator-=(
		const matrix3& other)
	{
		return *this = (*this) - other;
	}

	matrix3 matrix3::operator*=(
		const matrix3& other)
	{
		return *this = (*this) * other;
	}

	matrix3 matrix3::operator+(
		float other) const
	{
		return matrix3(
			rows[0] + other,
			rows[1] + other,
			rows[2] + other
		);
	}

	matrix3 matrix3::operator-(
		float other) const
	{
		return matrix3(
			rows[0] - other,
			rows[1] - other,
			rows[2] - other
		);
	}

	matrix3 matrix3::operator*(
		float other) const
	{
		return matrix3(
			rows[0] * other,
			rows[1] * other,
			rows[2] * other
		);
	}


	matrix3 matrix3::operator/(
		float other) const
	{
		return matrix3(
			rows[0] / other,
			rows[1] / other,
			rows[2] / other
		);
	}

	matrix3 matrix3::operator+=(
		float other)
	{
		return *this = (*this) + other;
	}

	matrix3 matrix3::operator-=(
		float other)
	{
		return *this = (*this) - other;
	}

	matrix3 matrix3::operator*=(
		float other)
	{
		return *this = (*this) * other;
	}

	matrix3 matrix3::operator/=(
		float other)
	{
		return *this = (*this) / other;
	}

	matrix3 matrix3::operator-() const
	{
		return matrix3(-rows[0], -rows[1], -rows[2]);
	}

	float& matrix3::operator()(
		int row,
		int col)
	{
		if (row > 2|| col > 2 || 0 > row || 0 > col) {
			throw std::out_of_range("Row/Col is outside the "
				"bounds of this matrix.");
		}

		return elements[col + row * 3];
	}

	const float& matrix3::operator()(
		int row,
		int col) const
	{
		if (row > 2 || col > 2 || 0 > row || 0 > col) {
			throw std::out_of_range("Row/Col is outside the "
				"bounds of this matrix.");
		}

		return elements[col + row * 3];
	}

	bool matrix3::operator==(
		const matrix3& other) const
	{
		return rows[0] == other.rows[0]
			&& rows[1] == other.rows[1]
			&& rows[2] == other.rows[2];
	}

	bool matrix3::operator!=(
		const matrix3& other) const
	{
		return !operator==(other);
	}

	matrix3 matrix3::from_axis_angle(
		const vector4& axis_angle)
	{
		return from_axis_angle(
			axis_angle.x, axis_angle.y, axis_angle.z, axis_angle.w);
	}

	matrix3 matrix3::from_axis_angle(
		const vector3& axis,
		float angle)
	{
		return from_axis_angle(axis.x, axis.y, axis.z, angle);
	}

	matrix3 matrix3::from_axis_angle(
		float x,
		float y,
		float z,
		float angle)
	{
		vector3 axis = vector3(x, y, z);
		axis.normalize();

		float cos = cosf(-angle);
		float sin = sinf(-angle);
		float t = 1.0f - cos;

		float tXX = t * axis.x * axis.x;
		float tXY = t * axis.x * axis.y;
		float tXZ = t * axis.x * axis.z;
		float tYY = t * axis.y * axis.y;
		float tYZ = t * axis.y * axis.z;
		float tZZ = t * axis.z * axis.z;

		float sinX = sin * axis.x;
		float sinY = sin * axis.y;
		float sinZ = sin * axis.z;

		return matrix3(
			tXX + cos, tXY + sinZ, tXZ - sinY,
			tXY - sinZ, tYY + cos, tYZ + sinX,
			tXZ + sinY, tYZ - sinX, tZZ + cos
		);
	}

	matrix3 matrix3::create_from_quaternion(
		const quaternion& rotation)
	{
		quaternion q = rotation;
		if (!almost_equal(q.length_squared(), 1, 6)) {
			q.normalize();
		}

		float xx = q.x * q.x;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float xw = q.x * q.w;
		float yy = q.y * q.y;
		float yz = q.y * q.z;
		float yw = q.y * q.w;
		float zz = q.z * q.z;
		float zw = q.z * q.w;

		float m00 = 1 - 2 * (yy + zz);
		float m01 = 2 * (xy - zw);
		float m02 = 2 * (xz + yw);
		float m10 = 2 * (xy + zw);
		float m11 = 1 - 2 * (xx + zz);
		float m12 = 2 * (yz - xw);
		float m20 = 2 * (xz - yw);
		float m21 = 2 * (yz + xw);
		float m22 = 1 - 2 * (xx + yy);

		return matrix3(
			m00, m01, m02,
			m10, m11, m12,
			m20, m21, m22
		);
	}

	matrix3 matrix3::create_rotation_x(
		float angle)
	{
		float cos = cosf(angle);
		float sin = sinf(angle);

		return matrix3(
			1, 0, 0,
			0, cos, -sin,
			0, sin, cos
		);
	}

	matrix3 matrix3::create_rotation_y(
		float angle)
	{
		float cos = cosf(angle);
		float sin = sinf(angle);

		return matrix3(
			cos, 0, sin,
			0, 1, 0,
			-sin, 0, cos
		);
	}

	matrix3 matrix3::create_rotation_z(
		float angle)
	{
		float cos = cosf(angle);
		float sin = sinf(angle);

		return matrix3(
			cos, -sin, 0,
			sin, cos, 0,
			0, 0, 1
		);
	}

	matrix3 matrix3::create_rotation(
		const vector3& angles)
	{
		return matrix3::create_rotation(angles.x, angles.y, angles.z);
	}

	matrix3 matrix3::create_rotation(
		float x,
		float y,
		float z)
	{
		return matrix3::create_rotation_x(x)
			 * matrix3::create_rotation_y(y)
			 * matrix3::create_rotation_z(z);
	}

	matrix3 matrix3::create_scale(
		float scale)
	{
		return create_scale(scale, scale, scale);
	}

	matrix3 matrix3::create_scale(
		const vector3& scale)
	{
		return create_scale(scale.x, scale.y, scale.z);
	}

	matrix3 matrix3::create_scale(
		float x,
		float y,
		float z)
	{
		return matrix3(
			x, 0, 0,
			0, y, 0,
			0, 0, z
		);
	}

	matrix3 matrix3::create_tensor(
		vector3 xyz, 
		float m)
	{
		return create_tensor(xyz.x, xyz.y, xyz.z, m);
	}

	matrix3 matrix3::create_tensor(
		float x, 
		float y,
		float z,
		float m)
	{
		float xx = m * (x * x);
		float yy = m * (y * y);
		float zz = m * (z * z);

		float xy = m * x * y;
		float xz = m * x * z;
		float yz = m * y * z;

		return matrix3(
			xx, xy, xz,
			xy, yy, yz,
			xz, yz, zz
		);
	}

	std::ostream& math::operator<<(
		std::ostream& stream,
		const matrix3& a)
	{
		return stream 
			<< a.rows[0] << std::endl
			<< a.rows[1] << std::endl
			<< a.rows[2];
	}
}
