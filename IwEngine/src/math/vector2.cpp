#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"

namespace iw {
	const vector2 vector2::zero   = vector2(0, 0);
	const vector2 vector2::one    = vector2(1, 1);
	const vector2 vector2::unit_x = vector2(1, 0);
	const vector2 vector2::unit_y = vector2(0, 1);

	vector2::vector2()
		: x(0)
		, y(0) {}

	vector2::vector2(
		float xy)
		: x(xy)
		, y(xy) {}

	vector2::vector2(
		const vector3& xy)
		: x(xy.x)
		, y(xy.y) {}

	vector2::vector2(
		const vector4& xy)
		: x(xy.x)
		, y(xy.y) {}

	vector2::vector2(
		float x,
		float y)
		: x(x)
		, y(y) {}

	float vector2::length() const {
		return sqrtf(x * x + y * y);
	}

	float vector2::length_squared() const {
		return x * x + y * y;
	}

	float vector2::length_fast() const {
		return 1 / fast_inv_sqrt(x * x + y * y);
	}

	float vector2::dot(
		const vector2 & other) const
	{
		return x * other.x + y * other.y;
	}

	float vector2::cross_length(
		const vector2 & other) const
	{
		return x * other.y - y * other.x;
	}

	vector2 vector2::normalized() const {
		vector2 tmp = *this;
		tmp.normalize();
		return tmp;
	}

	vector2 vector2::normalized_fast() const {
		vector2 tmp = *this;
		tmp.normalize_fast();
		return tmp;
	}

	void vector2::normalize() {
		float scale = length();
		if (scale == 0) {
			return;
		}

		x /= scale;
		y /= scale;
	}

	void vector2::normalize_fast() {
		float scale = length_fast();
		x /= scale;
		y /= scale;
	}

	float vector2::major() const {
		if (abs(x) > abs(y)) return x;
		else                 return y;
	}

	float vector2::minor() const {
		if (abs(x) < abs(y)) return x;
		else                 return y;
	}

	vector2 vector2::operator+(
		const vector2& other) const
	{
		return vector2(x + other.x, y + other.y);
	}

	vector2 vector2::operator-(
		const vector2& other) const
	{
		return vector2(x - other.x, y - other.y);
	}

	vector2 vector2::operator*(
		const vector2& other) const
	{
		return vector2(x * other.x, y * other.y);
	}

	vector2 vector2::operator/(
		const vector2& other) const
	{
		return vector2(x / other.x, y / other.y);
	}

	vector2 vector2::operator+=(
		const vector2& other)
	{
		return *this = (*this) + other;
	}

	vector2 vector2::operator-=(
		const vector2& other)
	{
		return *this = (*this) - other;
	}

	vector2 vector2::operator*=(
		const vector2& other)
	{
		return *this = (*this) * other;
	}

	vector2 vector2::operator/=(
		const vector2& other)
	{
		return *this = (*this) / other;
	}

	vector2 vector2::operator+(
		float other) const
	{
		return vector2(x + other, y + other);
	}

	vector2 vector2::operator-(
		float other) const
	{
		return vector2(x - other, y - other);
	}

	vector2 vector2::operator*(
		float other) const
	{
		return vector2(x * other, y * other);
	}

	vector2 vector2::operator/(
		float other) const
	{
		return vector2(x / other, y / other);
	}

	vector2 vector2::operator+=(
		float other)
	{
		return *this = (*this) + other;
	}

	vector2 vector2::operator-=(
		float other)
	{
		return *this = (*this) - other;
	}

	vector2 vector2::operator*=(
		float other)
	{
		return *this = (*this) * other;
	}

	vector2 vector2::operator/=(
		float other)
	{
		return *this = (*this) / other;
	}

	vector2 vector2::operator-() const {
		return vector2(-x, -y);
	}

	float& iw::vector2::operator[](
		size_t index)
	{
		if (index == 0) return x;
		else if (index == 1) return y;
		throw std::out_of_range("Index out of bounds");
	}

	bool vector2::operator==(
		const vector2& other) const
	{
		return x == other.x
			&& y == other.y;

		// should specialize almost_equal

		//return almost_equal(x, other.x, 6)
		//	&& almost_equal(y, other.y, 6);
	}

	bool vector2::operator!=(
		const vector2& other) const
	{
		return !operator==(other);
	}

	std::ostream& math::operator<<(
		std::ostream& ostream,
		const vector2& vector)
	{
		return ostream << "("
			<< vector.x << ", "
			<< vector.y << ")";
	}

	vector2 math::operator+(
		float left,
		const vector2& right)
	{
		return right + left;
	}

	vector2 math::operator*(
		float left,
		const vector2& right)
	{
		return right * left;
	}
}
