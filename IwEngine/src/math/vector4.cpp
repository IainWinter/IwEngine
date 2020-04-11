#include "iw/math/vector4.h"
#include "iw/math/vector2.h"
#include "iw/math/matrix4.h"
#include <cmath>

namespace iw {
	const vector4 vector4::zero   = vector4(0, 0, 0, 0);
	const vector4 vector4::one    = vector4(1, 1, 1, 1);
	const vector4 vector4::unit_x = vector4(1, 0, 0, 0);
	const vector4 vector4::unit_y = vector4(0, 1, 0, 0);
	const vector4 vector4::unit_z = vector4(0, 0, 1, 0);
	const vector4 vector4::unit_w = vector4(0, 0, 0, 1);

	vector4::vector4()
		: x(0)
		, y(0)
		, z(0)
		, w(0) {}

	vector4::vector4(
		float xyzw)
		: x(xyzw)
		, y(xyzw)
		, z(xyzw)
		, w(xyzw) {}

	vector4::vector4(
		const vector2& xy)
		: x(xy.x)
		, y(xy.y)
		, z(0)
		, w(0) {}

	vector4::vector4(
		const vector2& xy,
		float z,
		float w)
		: x(xy.x)
		, y(xy.y)
		, z(z)
		, w(w) {}

	vector4::vector4(
		const vector3& xyz)
		: x(xyz.x)
		, y(xyz.y)
		, z(xyz.z)
		, w(0) {}

	vector4::vector4(
		const vector3& xyz,
		float w)
	    : x(xyz.x)
		, y(xyz.y)
		, z(xyz.z)
		, w(w) {}

	vector4::vector4(
		float x,
		float y,
		float z,
		float w)
		: x(x)
		, y(y)
		, z(z)
		, w(w) {}

	float vector4::length() const {
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	float vector4::length_squared() const {
		return x * x + y * y + z * z + w * w;
	}

	float vector4::length_fast() const {
		return 1 / fast_inv_sqrt(x * x + y * y + z * z + w * w);
	}

	float vector4::dot(
		const vector4& other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	vector4 vector4::normalized() const {
		vector4 tmp = *this;
		tmp.normalize();
		return tmp;
	}

	vector4 vector4::normalized_fast() const {
		vector4 tmp = *this;
		tmp.normalize_fast();
		return tmp;
	}

	void vector4::normalize() {
		float scale = length();
		if (scale == 0) {
			return;
		}

		x /= scale;
		y /= scale;
		z /= scale;
		w /= scale;
	}

	void vector4::normalize_fast() {
		float scale = length_fast();
		x /= scale;
		y /= scale;
		z /= scale;
		w /= scale;
	}

	vector3 vector4::xyz() const {
		return vector3(x, y, z);
	}

	void vector4::xyz(
		const vector3& xyz)
	{
		vector4::xyz(xyz.x, xyz.y, xyz.z);
	}

	void vector4::xyz(
		float x,
		float y,
		float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float vector4::major() const {
		float major = x;
		if (y > major) major = y;
		if (z > major) major = z;
		if (w > major) major = w;

		return major;
	}

	float vector4::minor() const {
		float minor = x;
		if (y < minor) minor = y;
		if (z < minor) minor = z;
		if (w < minor) minor = w;

		return minor;
	}

	vector4 vector4::operator+(
		const vector4& other) const
	{
		return vector4(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	vector4 vector4::operator-(
		const vector4& other) const
	{
		return vector4(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	vector4 vector4::operator*(
		const vector4& other) const
	{
		return vector4(x * other.x, y * other.y, z * other.z, w * other.w);
	}

	vector4 vector4::operator/(
		const vector4& other) const
	{
		return vector4(x / other.x, y / other.y, z / other.z, w / other.w);
	}

	vector4& vector4::operator+=(
		const vector4& other)
	{
		return *this = (*this) + other;
	}

	vector4& vector4::operator-=(
		const vector4& other)
	{
		return *this = (*this) - other;
	}

	vector4& vector4::operator*=(
		const vector4& other)
	{
		return *this = (*this) * other;
	}

	vector4& vector4::operator/=(
		const vector4& other)
	{
		return *this = (*this) / other;
	}

	vector4 vector4::operator+(
		const float& other) const
	{
		return vector4(x + other, y + other, z + other, w + other);
	}

	vector4 vector4::operator-(
		const float& other) const
	{
		return vector4(x - other, y - other, z - other, w - other);
	}

	vector4 vector4::operator*(
		const float& other) const
	{
		return vector4(x * other, y * other, z * other, w * other);
	}

	vector4 vector4::operator/(
		const float& other) const
	{
		return vector4(x / other, y / other, z / other, w / other);
	}

	vector4 vector4::operator+=(
		const float& other)
	{
		return *this = (*this) + other;
	}

	vector4& vector4::operator-=(
		const float& other)
	{
		return *this = (*this) - other;
	}

	vector4& vector4::operator*=(
		const float& other)
	{
		return *this = (*this) * other;
	}

	vector4& vector4::operator/=(
		const float& other)
	{
		return *this = (*this) / other;
	}

	vector4 vector4::operator*(
		const matrix4& mat) const
	{
		return iw::vector4(
			x * mat(0, 0) + y * mat(1, 0) + z * mat(2, 0) + w * mat(3, 0),
			x * mat(0, 1) + y * mat(1, 1) + z * mat(2, 1) + w * mat(3, 1),
			x * mat(0, 2) + y * mat(1, 2) + z * mat(2, 2) + w * mat(3, 2),
			x * mat(0, 3) + y * mat(1, 3) + z * mat(2, 3) + w * mat(3, 3));
	}

	vector4& vector4::operator*=(
		const matrix4& mat)
	{
		return *this = iw::vector4(
			x * mat(0, 0) + y * mat(1, 0) + z * mat(2, 0) + w * mat(3, 0),
			x * mat(0, 1) + y * mat(1, 1) + z * mat(2, 1) + w * mat(3, 1),
			x * mat(0, 2) + y * mat(1, 2) + z * mat(2, 2) + w * mat(3, 2),
			x * mat(0, 3) + y * mat(1, 3) + z * mat(2, 3) + w * mat(3, 3));
	}

	vector4 vector4::operator-() const {
		return vector4(-x, -y, -z, -w);
	}

	float& iw::vector4::operator[](
		size_t index)
	{
		if (index == 0) return x;
		else if (index == 1) return y;
		else if (index == 2) return z;
		else if (index == 3) return w;
		throw std::out_of_range("Index out of bounds");
	}

	bool vector4::operator==(
		const vector4& other) const
	{
		return almost_equal(x, other.x, 6)
			&& almost_equal(y, other.y, 6)
			&& almost_equal(z, other.z, 6)
			&& almost_equal(w, other.w, 6);
	}

	bool vector4::operator!=(
		const vector4& other) const
	{
		return !operator==(other);
	}

	std::ostream& math::operator<<(
		std::ostream& ostream,
		const vector4& vector)
	{
		return ostream << "(" 
			<< vector.x << ", " 
			<< vector.y << ", " 
			<< vector.z << ", " 
			<< vector.w << ")";
	}

	vector4 math::operator+(
		float left,
		const vector4& right)
	{
		return right + left;
	}

	vector4 math::operator*(
		float left,
		const vector4& right)
	{
		return right * left;
	}
}
