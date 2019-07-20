#include "iw/math/vector3.h"
#include "iw/math/vector4.h"
#include "iw/math/matrix3.h"
#include "iw/math/matrix4.h"

namespace iwmath {
	const vector3 vector3::zero   = vector3(0, 0, 0);
	const vector3 vector3::one    = vector3(1, 1, 1);
	const vector3 vector3::unit_x = vector3(1, 0, 0);
	const vector3 vector3::unit_y = vector3(0, 1, 0);
	const vector3 vector3::unit_z = vector3(0, 0, 1);

	vector3::vector3()
		: xy(0)
		, z(0) {}

	vector3::vector3(
		float xyz)
		: xy(xyz)
		, z(xyz) {}

	vector3::vector3(
		float x,
		float y,
		float z)
		: xy(x, y)
		, z(z) {}

	float vector3::length() const {
		return sqrtf(xy.x * xy.x + xy.y * xy.y + z * z);
	}

	float vector3::length_squared() const {
		return xy.x * xy.x + xy.y * xy.y + z * z;
	}

	float vector3::length_fast() const {
		return 1 / inv_sqrt(xy.x * xy.x + xy.y * xy.y + z * z);
	}

	float vector3::dot(
		const vector3& other) const
	{
		return xy.x * other.xy.x + xy.y * other.xy.y + z * other.z;
	}

	vector3 vector3::cross(
		const vector3& other) const
	{
		return vector3(
			xy.y * other.z     - z   * other.xy.y,
			z    * other.xy.x - xy.x * other.z,
			xy.x * other.xy.y - xy.y * other.xy.x);
	}

	vector3 vector3::normalized() const {
		vector3 tmp = *this;
		tmp.normalize();
		return tmp;
	}

	vector3 vector3::normalized_fast() const {
		vector3 tmp = *this;
		tmp.normalize_fast();
		return tmp;
	}

	void vector3::normalize() {
		float scale = length();
		if (scale == 0) {
			return;
		}

		xy /= scale;
		z  /= scale;
	}

	void vector3::normalize_fast() {
		float scale = length_fast();
		if (scale == 0) {
			return;
		}

		xy /= scale;
		z  /= scale;
	}

	float& vector3::operator[](
		std::size_t index)
	{
		switch (index) {
			case 0:  return xy.x;
			case 1:  return xy.y;
			case 2:  return z;
			default: throw std::out_of_range("Index out of bounds");
		}
	}

	vector3 vector3::operator+(
		const vector3& other) const
	{
		return vector3(xy.x + other.xy.x, xy.y + other.xy.y, z + other.z);
	}

	vector3 vector3::operator-(
		const vector3& other) const
	{
		return vector3(xy.x - other.xy.x, xy.y - other.xy.y, z - other.z);
	}

	vector3 vector3::operator*(
		const vector3& other) const
	{
		return vector3(xy.x * other.xy.x, xy.y * other.xy.y, z * other.z);
	}

	vector3 vector3::operator/(
		const vector3& other) const
	{
		return vector3(xy.x / other.xy.x, xy.y / other.xy.y, z / other.z);
	}

	vector3 vector3::operator+=(
		const vector3& other)
	{
		return *this = (*this) + other;
	}

	vector3 vector3::operator-=(
		const vector3& other)
	{
		return *this = (*this) - other;
	}

	vector3 vector3::operator*=(
		const vector3& other)
	{
		return *this = (*this) * other;
	}

	vector3 vector3::operator/=(
		const vector3& other)
	{
		return *this = (*this) / other;
	}

	vector3 vector3::operator+(
		float other) const
	{
		return vector3(xy.x + other, xy.y + other, z + other);
	}

	vector3 vector3::operator-(
		float other) const
	{
		return vector3(xy.x - other, xy.y - other, z - other);
	}

	vector3 vector3::operator*(
		float other) const
	{
		return vector3(xy.x * other, xy.y * other, z * other);
	}

	vector3 vector3::operator/(
		float other) const
	{
		return vector3(xy.x / other, xy.y / other, z / other);
	}

	vector3 vector3::operator+=(
		float other)
	{
		return *this = (*this) + other;
	}

	vector3 vector3::operator-=(
		float other)
	{
		return *this = (*this) - other;
	}

	vector3 vector3::operator*=(
		float other)
	{
		return *this = (*this) * other;
	}

	vector3 vector3::operator/=(
		float other)
	{
		return *this = (*this) / other;
	}

	vector3 vector3::operator*(
		const matrix3& mat) const
	{
		return iwm::vector3(
			xy.x * mat(0, 0) + xy.y * mat(0, 1) + z * mat(0, 2),
			xy.x * mat(1, 0) + xy.y * mat(1, 1) + z * mat(1, 2),
			xy.x * mat(2, 0) + xy.y * mat(2, 1) + z * mat(2, 2));
	}

	vector3& vector3::operator*=(
		const matrix3& mat)
	{
		return *this = iwm::vector3(
			xy.x * mat(0, 0) + xy.y * mat(0, 1) + z * mat(0, 2),
			xy.x * mat(1, 0) + xy.y * mat(1, 1) + z * mat(1, 2),
			xy.x * mat(2, 0) + xy.y * mat(2, 1) + z * mat(2, 2));
	}

	vector3 vector3::operator*(
		const matrix4& mat) const
	{
		return (vector4(*this, 1) * mat).xyz;
	}

	vector3& vector3::operator*=(
		const matrix4& mat)
	{
		return *this = (vector4(*this, 1) * mat).xyz;
	}

	vector3 vector3::operator-() const {
		return vector3(-xy.x, -xy.y, -z);
	}

	bool vector3::operator==(
		const vector3& other) const
	{
		return almost_equal(xy.x, other.xy.x, 6)
			&& almost_equal(xy.y, other.xy.y, 6)
			&& almost_equal(z, other.z, 6);
	}

	bool vector3::operator!=(
		const vector3& other) const
	{
		return !operator==(other);
	}

	std::ostream& iwmath::operator<<(
		std::ostream& ostream,
		const vector3& vector)
	{
		return ostream << "("
			<< vector.xy.x << ", "
			<< vector.xy.y << ", "
			<< vector.z << ")";
	}

	vector3 iwmath::operator+(
		float left,
		const vector3& right)
	{
		return right + left;
	}

	vector3 iwmath::operator*(
		float left,
		const vector3& right)
	{
		return right * left;
	}
}
