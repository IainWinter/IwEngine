#include <cmath>
#include "iw/math/vector4.h"
#include "iw/math/matrix4.h"

namespace iwmath {
	const vector4 vector4::zero   = vector4(0, 0, 0, 0);
	const vector4 vector4::one    = vector4(1, 1, 1, 1);
	const vector4 vector4::unit_x = vector4(1, 0, 0, 0);
	const vector4 vector4::unit_y = vector4(0, 1, 0, 0);
	const vector4 vector4::unit_z = vector4(0, 0, 1, 0);
	const vector4 vector4::unit_w = vector4(0, 0, 0, 1);

	vector4::vector4()
		: xyz(0)
		, w(0) {}

	vector4::vector4(
		float xyzw)
		: xyz(0)
		, w(xyzw) {}

	vector4::vector4(
		const vector3& xyz)
		: xyz(xyz)
		, w(0) {}

	vector4::vector4(
		const vector3& xyz,
		float w)
	    : xyz(xyz)
		, w(w) {}

	vector4::vector4(
		float x,
		float y,
		float z,
		float w)
		: xyz(x, y, z)
		, w(w) {}

	float vector4::length() const {
		return sqrtf(xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z + w * w);
	}

	float vector4::length_squared() const {
		return xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z + w * w;
	}

	float vector4::length_fast() const {
		return 1 / inv_sqrt(xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z + w * w);
	}

	float vector4::dot(
		const vector4& other) const
	{
		return xyz.x * other.xyz.x + xyz.y * other.xyz.y + xyz.z * other.xyz.z + w * other.w;
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

		xyz /= scale;
		w   /= scale;
	}

	void vector4::normalize_fast() {
		float scale = length_fast();
		xyz /= scale;
		w   /= scale;
	}

	float& iwmath::vector4::operator[](
		std::size_t index)
	{
		switch (index) {
			case 0:  return xyz.x;
			case 1:  return xyz.y;
			case 2:  return xyz.z;
			case 3:  return w;
			default: throw std::out_of_range("Index out of bounds");
		}
	}

	vector4 vector4::operator+(
		const vector4& other) const
	{
		return vector4(xyz.x + other.xyz.x, xyz.y + other.xyz.y, xyz.z + other.xyz.z, w + other.w);
	}

	vector4 vector4::operator-(
		const vector4& other) const
	{
		return vector4(xyz.x - other.xyz.x, xyz.y - other.xyz.y, xyz.z - other.xyz.z, w - other.w);
	}

	vector4 vector4::operator*(
		const vector4& other) const
	{
		return vector4(xyz.x * other.xyz.x, xyz.y * other.xyz.y, xyz.z * other.xyz.z, w * other.w);
	}

	vector4 vector4::operator/(
		const vector4& other) const
	{
		return vector4(xyz.x / other.xyz.x, xyz.y / other.xyz.y, xyz.z / other.xyz.z, w / other.w);
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
		return vector4(xyz.x + other, xyz.y + other, xyz.z + other, w + other);
	}

	vector4 vector4::operator-(
		const float& other) const
	{
		return vector4(xyz.x - other, xyz.y - other, xyz.z - other, w - other);
	}

	vector4 vector4::operator*(
		const float& other) const
	{
		return vector4(xyz.x * other, xyz.y * other, xyz.z * other, w * other);
	}

	vector4 vector4::operator/(
		const float& other) const
	{
		return vector4(xyz.x / other, xyz.y / other, xyz.z / other, w / other);
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
		return iwm::vector4(
			xyz.x * mat(0, 0) + xyz.y * mat(1, 0) + xyz.z * mat(2, 0) + w * mat(3, 0),
			xyz.x * mat(0, 1) + xyz.y * mat(1, 1) + xyz.z * mat(2, 1) + w * mat(3, 1),
			xyz.x * mat(0, 2) + xyz.y * mat(1, 2) + xyz.z * mat(2, 2) + w * mat(3, 2),
			xyz.x * mat(0, 3) + xyz.y * mat(1, 3) + xyz.z * mat(2, 3) + w * mat(3, 3));
	}

	vector4& vector4::operator*=(
		const matrix4& mat)
	{
		return *this = iwm::vector4(
			xyz.x * mat(0, 0) + xyz.y * mat(1, 0) + xyz.z * mat(2, 0) + w * mat(3, 0),
			xyz.x * mat(0, 1) + xyz.y * mat(1, 1) + xyz.z * mat(2, 1) + w * mat(3, 1),
			xyz.x * mat(0, 2) + xyz.y * mat(1, 2) + xyz.z * mat(2, 2) + w * mat(3, 2),
			xyz.x * mat(0, 3) + xyz.y * mat(1, 3) + xyz.z * mat(2, 3) + w * mat(3, 3));
	}

	vector4 vector4::operator-() const {
		return vector4(-xyz, -w);
	}

	bool vector4::operator==(
		const vector4& other) const
	{
		return almost_equal(xyz.x, other.xyz.x, 6)
			&& almost_equal(xyz.y, other.xyz.y, 6)
			&& almost_equal(xyz.z, other.xyz.z, 6)
			&& almost_equal(w, other.w, 6);
	}

	bool vector4::operator!=(
		const vector4& other) const
	{
		return !operator==(other);
	}

	std::ostream& iwmath::operator<<(
		std::ostream& ostream,
		const vector4& vector)
	{
		return ostream << "(" 
			<< vector.xyz.x << ", " 
			<< vector.xyz.y << ", " 
			<< vector.xyz.z << ", " 
			<< vector.w << ")";
	}

	vector4 iwmath::operator+(
		float left,
		const vector4& right)
	{
		return right + left;
	}

	vector4 iwmath::operator*(
		float left,
		const vector4& right)
	{
		return right * left;
	}
}
