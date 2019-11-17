#include "iw/math/quaternion.h"
#include "iw/math/matrix3.h"

namespace iw {
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
		return 1 / fast_inv_sqrt(x * x + y * y + z * z + w * w);
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

	quaternion quaternion::from_axis_angle(
		const vector4& axis_angle)
	{
		return from_axis_angle(
			axis_angle.x, axis_angle.y, axis_angle.z, axis_angle.w);
	}

	quaternion quaternion::from_axis_angle(
		const vector3& axis,
		float angle)
	{
		return from_axis_angle(axis.x, axis.y, axis.z, angle);
	}

	quaternion quaternion::from_axis_angle(
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

	quaternion quaternion::from_euler_angles(
		const vector3& angles)
	{
		return from_euler_angles(angles.x, angles.y, angles.z);
	}

	quaternion quaternion::from_euler_angles(
		float x,
		float y,
		float z)
	{
		float cy = cosf(z * 0.5f);
		float sy = sinf(z * 0.5f);
		float cp = cosf(y * 0.5f);
		float sp = sinf(y * 0.5f);
		float cr = cosf(x * 0.5f);
		float sr = sinf(x * 0.5f);

		return quaternion(
			cy * cp * sr - sy * sp * cr,
			sy * cp * sr + cy * sp * cr,
			sy * cp * cr - cy * sp * sr,
			cy * cp * cr + sy * sp * sr);

		//x /= 2;
		//y /= 2;
		//z /= 2;

		//float cx = cosf(x);
		//float sx = sinf(x);
		//float cy = cosf(y);
		//float sy = sinf(y);
		//float cz = cosf(z);
		//float sz = sinf(z);

		//return quaternion(
		//	cx * cy * cz + sx * sy * sz,
		//	sx * cy * cz - cx * sy * sz,
		//	cx * sy * cz + sx * cy * sz,
		//	cx * cy * sz - sx * sy * cz
		//);
	}

	quaternion quaternion::from_look_at(
		vector3 eye,
		vector3 target,
		vector3 up)
	{
		vector3 forward = (target - eye).normalized();
		float dot = forward.dot(vector3::unit_z);

		if (iw::almost_equal(dot, -1.0f, 6.0f)) {
			return quaternion(up, PI);
		}

		else if (iw::almost_equal(dot, 1.0f, 6.0f)) {
			return identity;
		}

		float angle = acos(dot);
		vector3 axis = forward.cross(vector3::unit_z).normalized();

		return from_axis_angle(axis, angle);
	}

	std::ostream& iw::operator<<(
		std::ostream& ostream,
		const quaternion& quaternion)
	{
		return ostream << "("
			<< quaternion.x << ", "
			<< quaternion.y << ", "
			<< quaternion.z << ", "
			<< quaternion.w << ")";
	}
}
