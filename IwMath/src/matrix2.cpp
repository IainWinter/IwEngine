#include "matrix2.h"

namespace iwmath {
	const matrix2 matrix2::identity = matrix2(1.0f);

	matrix2::matrix2() {
		memset(elements, 0, 2 * 2 * sizeof(float));
	}

	matrix2::matrix2(float diagonal) {
		memset(elements, 0, 2 * 2 * sizeof(float));
		elements[0 + 0 * 2] = diagonal;
		elements[1 + 1 * 2] = diagonal;
	}

	matrix2::matrix2(float* elements) {
		memcpy(this->elements, elements, 2 * 2 * sizeof(float));
	}

	matrix2::matrix2(vector2 row0, vector2 row1) {
		rows[0] = row0;
		rows[1] = row1;
	}

	matrix2::matrix2(float m00, float m01, float m10, float m11) {
		elements[0] = m00;
		elements[1] = m01;
		elements[2] = m10;
		elements[3] = m11;
	}

	float matrix2::determinant() const {
		return rows[0].x * rows[1].y - rows[0].y * rows[1].x;
	}

	float matrix2::trace() const {
		return rows[0].x + rows[1].y;
	}

	matrix2 matrix2::transposed() const {
		matrix2 m = *this;
		m.transpose();
		return m;
	}

	matrix2 matrix2::inverted() const {
		matrix2 m = *this;
		m.invert();
		return m;
	}

	matrix2 matrix2::normalized() const {
		matrix2 m = *this;
		m.normalize();
		return m;
	}

	void matrix2::transpose() {
		matrix2 tmp = *this;
		rows[0].y = tmp.rows[1].x;
		rows[1].x = tmp.rows[0].y;
	}


	void matrix2::invert() {
		float det = determinant();
		if (almost_equal(det, 0, 6)) {
			std::invalid_argument("Determinant is zero, therefore inverse matrix doesn't exist.");
		}

		float invDet = 1.0f / det;

		matrix2 tmp = *this;
		rows[0].x = tmp.rows[1].y * invDet;
		rows[0].y = -tmp.rows[0].y * invDet;
		rows[1].x = -tmp.rows[1].x * invDet;
		rows[1].y = tmp.rows[0].x * invDet;
	}

	void matrix2::normalize() {
		float det = determinant();
		if (almost_equal(det, 0, 6)) {
			return;
		}

		*this /= det;
	}

	matrix2 matrix2::operator+(const matrix2& other) const {
		return matrix2(
			rows[0] + other.rows[0],
			rows[1] + other.rows[1]
		);
	}

	matrix2 matrix2::operator-(const matrix2& other) const {
		return matrix2(
			rows[0] - other.rows[0],
			rows[1] - other.rows[1]
		);
	}

	matrix2 matrix2::operator*(const matrix2& other) const {
		float a00 = rows[0].x;
		float a10 = rows[0].y;
		float a01 = rows[1].x;
		float a11 = rows[1].y;

		float b00 = other.rows[0].x;
		float b10 = other.rows[0].y;
		float b01 = other.rows[1].x;
		float b11 = other.rows[1].y;

		return matrix2(
			a00 * b00 + a10 * b01,
			a00 * b10 + a10 * b11,
			a01 * b00 + a11 * b01,
			a01 * b10 + a11 * b11
		);
	}

	matrix2 matrix2::operator+=(const matrix2& other) {
		return *this = *this + other;
	}

	matrix2 matrix2::operator-=(const matrix2& other) {
		return *this = *this - other;
	}

	matrix2 matrix2::operator*=(const matrix2& other) {
		return *this = *this * other;
	}

	matrix2 matrix2::operator+(float other) const {
		return matrix2(
			rows[0] + other,
			rows[1] + other
		);
	}

	matrix2 matrix2::operator-(float other) const {
		return matrix2(
			rows[0] - other,
			rows[1] - other
		);
	}

	matrix2 matrix2::operator*(float other) const {
		return matrix2(
			rows[0] * other,
			rows[1] * other
		);
	}

	matrix2 matrix2::operator/(float other) const {
		return matrix2(
			rows[0] / other,
			rows[1] / other
		);
	}

	matrix2 matrix2::operator+=(float other) {
		return *this = *this + other;
	}

	matrix2 matrix2::operator-=(float other) {
		return *this = *this - other;
	}

	matrix2 matrix2::operator*=(float other) {
		return *this = *this * other;
	}

	matrix2 matrix2::operator/=(float other) {
		return *this = *this / other;
	}

	matrix2 matrix2::operator-() const {
		return matrix2(-rows[0], -rows[1]);
	}

	float& matrix2::operator()(int row, int col) {
		if (row > 2 || col > 2 || 0 > row || 0 > col) {
			throw std::out_of_range("Row/Col is outside the bounds of this maxtrix.");
		}

		return elements[col + row * 2];
	}

	bool matrix2::operator==(const matrix2& other) const {
		return rows[0] == other.rows[0] && rows[1] == other.rows[1];
	}

	bool matrix2::operator!=(const matrix2& other) const {
		return !operator==(other);
	}

	matrix2 matrix2::create_rotation(float angle) {
		float cos = cosf(angle);
		float sin = sinf(angle);

		return matrix2(
			cos, -sin,
			sin, cos
		);
	}

	matrix2 matrix2::create_scale(float scale) {
		return create_scale(scale, scale);
	}

	matrix2 matrix2::create_scale(const vector2& scale) {
		return create_scale(scale.x, scale.y);
	}

	matrix2 matrix2::create_scale(float x, float y) {
		return matrix2(
			x, 0,
			0, y
		);
	}

	std::ostream& iwmath::operator<<(std::ostream& ostream, const matrix2& a) {
		return ostream << a.rows[0] << std::endl << a.rows[1];
	}
}