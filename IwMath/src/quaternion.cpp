#include "quaternion.h"
#include "matrix3.h"

namespace iwmath {
	const quaternion quaternion::identity = quaternion(0, 0, 0, 1);

	quaternion::quaternion()
		: x(0)
		, y(0)
		, z(0)
		, w(1) {}

	quaternion::quaternion(
		float x,
		float y,
		float z,
		float w)
		: x(x)
		, y(y)
		, z(z)
		, w(w) {}

	quaternion::quaternion(
		const vector3& xyz,
		float w)
		: x(xyz.x)
		, y(xyz.y)
		, z(xyz.z)
		, w(w) {}

	quaternion::quaternion(
		const vector4& xyzw)
		: x(xyzw.x)
		, y(xyzw.y)
		, z(xyzw.z)
		, w(xyzw.w) {}

	float quaternion::length() const {
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	float quaternion::length_squared() const {
		return x * x + y * y + z * z + w * w;
	}

	float quaternion::length_fast() const {
		return 1 / inv_sqrt(x * x + y * y + z * z + w * w);
	}

	quaternion quaternion::normalized() const {
		quaternion tmp = *this;
		tmp.normalize();
		return tmp;
	}

	quaternion quaternion::normalized_fast() const {
		quaternion tmp = *this;
		tmp.normalize_fast();
		return tmp;
	}

	quaternion quaternion::inverted() const {
		quaternion tmp = *this;
		tmp.invert();
		return tmp;
	}

	quaternion quaternion::conjugated() const {
		quaternion tmp = *this;
		tmp.conjugate();
		return tmp;
	}

	void quaternion::normalize() {
		float scale = length();
		if (scale == 0) {
			return;
		}

		x /= scale;
		y /= scale;
		z /= scale;
		w /= scale;
	}

	void quaternion::normalize_fast() {
		float scale = length_fast();
		if (almost_equal(scale, 0, 6)) {
			return;
		}

		x /= scale;
		y /= scale;
		z /= scale;
		w /= scale;
	}

	void quaternion::invert() {
		float ls = length_squared();
		if (almost_equal(ls, 0, 6)) {
			return;
		}

		x = -x / ls;
		y = -y / ls;
		z = -z / ls;
		w = w / ls;
	}

	void quaternion::conjugate() {
		x = -x;
		y = -y;
		z = -z;
	}

	vector4 quaternion::axis_angle() const {
		quaternion q = *this;
		if (!almost_equal(q.length_squared(), 1, 6)) {
			q.normalize();
		}

		vector3 xyz = vector3(q.x, q.y, q.z);
		float length = xyz.length();

		if (almost_equal(length, 0, 6)) {
			return vector4::zero;
		}

		return vector4(xyz / length, 2 * atan2f(length, q.w));
	}

	vector3 quaternion::euler_angles() const {
		quaternion q = *this;
		if (!almost_equal(q.length_squared(), 1, 6)) {
			q.normalize();
		}

		float vz = atan2f(
			-2 * (q.x * q.y - q.w * q.z), 
			q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);

		float vy = asinf(2 * (q.x * q.z + q.w * q.y));

		float vx = atan2f(
			-2 * (q.y * q.z - q.w * q.x),
			q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);

		return vector3(vx, vy, vz);
	}

	quaternion quaternion::operator+(
		const quaternion& other) const
	{
		return quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	quaternion quaternion::operator-(
		const quaternion& other) const
	{
		return quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	quaternion quaternion::operator*(
		const quaternion& other) const
	{
		return quaternion(
			other.w * x + other.x * w - other.y * z + other.z * y,
			other.w * y + other.x * z + other.y * w - other.z * x,
			other.w * z - other.x * y + other.y * x + other.z * w,
			other.w * w - other.x * x - other.y * y - other.z * z);
	}

	quaternion quaternion::operator+=(
		const quaternion& other)
	{
		return *this = *this + other;
	}

	quaternion quaternion::operator-=(
		const quaternion& other)
	{
		return *this = *this - other;
	}

	quaternion quaternion::operator*=(
		const quaternion& other)
	{
		return *this = *this * other;
	}

	quaternion quaternion::operator*(
		float other) const
	{
		return quaternion(x * other, y * other, z * other, w * other);
	}

	quaternion quaternion::operator*=(
		float other)
	{
		return *this = *this * other;
	}

	bool quaternion::operator==(
		const quaternion& other) const
	{
		return almost_equal(x, other.x, 6)
			&& almost_equal(y, other.y, 6)
			&& almost_equal(z, other.z, 6)
			&& almost_equal(w, other.w, 6);
	}

	bool quaternion::operator!=(
		const quaternion& other) const
	{
		return !operator==(other);
	}

	quaternion quaternion::create_from_axis_angle(
		const vector4& axis_angle)
	{
		return create_from_axis_angle(
			axis_angle.x, axis_angle.y, axis_angle.z, axis_angle.w);
	}

	quaternion quaternion::create_from_axis_angle(
		const vector3& axis,
		float angle)
	{
		return create_from_axis_angle(axis.x, axis.y, axis.z, angle);
	}

	quaternion quaternion::create_from_axis_angle(
		float x,
		float y,
		float z,
		float angle)
	{
		vector3 axis = vector3(x, y, z);
		float length = axis.length_squared();

		if (length != 1) {
			axis.normalize();
		}

		else if (length == 0) {
			return identity;
		}

		float a = angle / 2;
		return quaternion(axis * sin(a), cos(a));
	}

	quaternion quaternion::create_from_euler_angles(
		const vector3& angles)
	{
		return create_from_euler_angles(angles.x, angles.y, angles.z);
	}

	quaternion quaternion::create_from_euler_angles(
		float x,
		float y,
		float z)
	{
		x /= 2;
		y /= 2;
		z /= 2;

		float cx = cosf(x);
		float sx = sinf(x);
		float cy = cosf(y);
		float sy = sinf(y);
		float cz = cosf(z);
		float sz = sinf(z);

		return quaternion(
			cz * cy * sx + sz * sy * cx,
			cz * sy * cx - sz * cy * sx,
			sz * cy * cx + cz * sy * sx,
			cz * cy * cx - sz * sy * sx
		);
	}

	std::ostream& iwmath::operator<<(
		std::ostream& ostream,
		const quaternion& quaternion)
	{
		return ostream << "("
			<< quaternion.x << ", "
			<< quaternion.y << ", "
			<< quaternion.z << ", "
			<< quaternion.w << ")";
	}

	vector3 iwmath::operator*(
		const iwmath::vector3& left,
		const quaternion& right)
	{
		vector3 v = vector3(right.x, right.y, right.z);
		float s = right.w;

		return  2.0f * v.dot(left) * v
			+ (s*s - v.dot(v)) * left
			+ 2.0f * s * v.cross(left);
	}
}