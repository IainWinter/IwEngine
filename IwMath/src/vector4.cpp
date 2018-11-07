#include <cmath>
#include "vector4.h"

namespace iwmath {
	const vector4 vector4::zero   = vector4(0, 0, 0, 0);
	const vector4 vector4::one	  = vector4(1, 1, 1, 1);
	const vector4 vector4::unit_x = vector4(1, 0, 0, 0);
	const vector4 vector4::unit_y = vector4(0, 1, 0, 0);
	const vector4 vector4::unit_z = vector4(0, 0, 1, 0);
	const vector4 vector4::unit_w = vector4(0, 0, 0, 1);

	vector4::vector4()
		: x(0), y(0), z(0), w(0) {
	}

	vector4::vector4(float xyzw)
		: x(xyzw), y(xyzw), z(xyzw), w(xyzw) {
	}

	vector4::vector4(const vector3& xyz)
		: x(xyz.x), y(xyz.y), z(xyz.z), w(0) {
	}

	vector4::vector4(const vector3& xyz, float w)
		: x(xyz.x), y(xyz.y), z(xyz.z), w(w) {
	}

	vector4::vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w) {
	}

	float vector4::length() const {
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	float vector4::length_squared() const {
		return x * x + y * y + z * z + w * w;
	}

	float vector4::length_fast() const {
		return 1 / inv_sqrt(x * x + y * y + z * z + w * w);
	}

	float vector4::dot(const vector4& other) const {
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

	void vector4::xyz(const vector3& xyz) {
		vector4::xyz(xyz.x, xyz.y, xyz.z);
	}

	void vector4::xyz(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float& iwmath::vector4::operator[](std::size_t index) {
		if (index == 0) return x;
		else if (index == 1) return y;
		else if (index == 2) return z;
		else if (index == 3) return w;
		throw std::out_of_range("Index out of bounds");
	}

	vector4 vector4::operator+(const vector4& other) const {
		return vector4(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	vector4 vector4::operator-(const vector4& other) const {
		return vector4(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	vector4 vector4::operator*(const vector4& other) const {
		return vector4(x * other.x, y * other.y, z * other.z, w * other.w);
	}

	vector4 vector4::operator/(const vector4& other) const {
		return vector4(x / other.x, y / other.y, z / other.z, w / other.w);
	}

	vector4 vector4::operator+=(const vector4& other) {
		return *this = (*this) + other;
	}

	vector4 vector4::operator-=(const vector4& other) {
		return *this = (*this) - other;
	}

	vector4 vector4::operator*=(const vector4& other) {
		return *this = (*this) * other;
	}

	vector4 vector4::operator/=(const vector4& other) {
		return *this = (*this) / other;
	}

	vector4 vector4::operator+(const float& other) const {
		return vector4(x + other, y + other, z + other, w + other);
	}

	vector4 vector4::operator-(const float& other) const {
		return vector4(x - other, y - other, z - other, w - other);
	}

	vector4 vector4::operator*(const float& other) const {
		return vector4(x * other, y * other, z * other, w * other);
	}

	vector4 vector4::operator/(const float& other) const {
		return vector4(x / other, y / other, z / other, w / other);
	}

	vector4 vector4::operator+=(const float& other) {
		return *this = (*this) + other;
	}

	vector4 vector4::operator-=(const float& other) {
		return *this = (*this) - other;
	}

	vector4 vector4::operator*=(const float& other) {
		return *this = (*this) * other;
	}

	vector4 vector4::operator/=(const float& other) {
		return *this = (*this) / other;
	}

	vector4 vector4::operator-() const {
		return vector4(-x, -y, -z, -w);
	}

	bool vector4::operator==(const vector4& other) const {
		return almost_equal(x, other.x, 6) && almost_equal(y, other.y, 6) && almost_equal(z, other.z, 6) && almost_equal(w, other.w, 6);
	}

	bool vector4::operator!=(const vector4& other) const {
		return !operator==(other);
	}

	std::ostream& iwmath::operator<<(std::ostream& ostream, const vector4& vector) {
		return ostream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
	}

	vector4 iwmath::operator+(float left, const vector4& right) {
		return right + left;
	}

	vector4 iwmath::operator*(float left, const vector4& right) {
		return right * left;
	}
}