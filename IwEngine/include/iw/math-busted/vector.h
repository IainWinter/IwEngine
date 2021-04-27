//#pragma once
//
//#include "iwmath.h"
//#include <ostream>
//#include <array>
//
//namespace iw {
//namespace math {
//	template<
//		size_t   _count,
//		typename _t = float>
//	struct REFLECT vector {
//		//static const vector zero, one;
//
//		REFLECT _t components[_count];
//
//		vector()		{ for (_t& c : components) c = 0;   }
//		vector(_t all) { for (_t& c : components) c = all; }
//
//		// replace with variatic?
//
//		template<
//			size_t _c = _count,
//			typename = typename std::enable_if_t<_c == 2>>
//		vector(_t x, _t y) {
//			components[0] = x;
//			components[1] = y;
//		}
//
//		template<
//			size_t _c = _count,
//			typename = typename std::enable_if_t<_c == 3>>
//		vector(_t x, _t y, _t z) {
//			components[0] = x;
//			components[1] = y;
//			components[2] = z;
//		}
//
//		template<
//			size_t _c = _count,
//			typename = typename std::enable_if_t<_c == 4>>
//		vector(_t x, _t y, _t z, _t w) {
//			components[0] = x;
//			components[1] = y;
//			components[2] = z;
//			components[3] = w;
//		}
//
//		void reset(
//			_t value = 0)
//		{
//			for (int i = 0; i < _count; i++) {
//				components[i] = value;
//			}
//		}
//
//		_t major() const {
//			_t major = 0;
//			for (_t& c : components) {
//				if (abs(c) > abs(major)) {
//					major = c;
//				}
//			}
//			return major;
//		}
//
//		_t minor() const {
//			_t minor = 0;
//			for (_t& c : components) {
//				if (abs(c) < abs(minor)) {
//					minor = c;
//				}
//			}
//			return minor;
//		}
//
//		_t length()         const { return sqrt(length_squared()); }
//		_t length_fast()    const { return _t(1) / fast_inv_sqrt(length_squared()); }
//		_t length_squared() const { return dot(*this); }
//
//		_t dot(
//			const vector& other) const
//		{
//			_t length = 0;
//			for (size_t i = 0; i < _count; i++) {
//				length += components[i] * other.components[i];
//			}
//
//			return length;
//		}
//
//		//_t cross_length(
//		//	const vector& other) const
//		//{
//		//	// TODO: impl
//		//	return 0;
//		//}
//
//		//vector cross(
//		//	const vector& other) const
//		//{
//		//	// todo: impl https://math.stackexchange.com/questions/752774/generalized-cross-product
//		//	return 0;
//		//}
//
//		void normalize() {
//			_t scale = length();
//			for (_t& c : components) {
//				c /= scale;
//			}
//		}
//
//		void normalize_fast() {
//			_t scale = length_fast();
//			for (_t& c : components) {
//				c /= scale;
//			}
//		}
//
//		vector normalized() const {
//			vector tmp;
//			tmp.normalize();
//			return tmp;
//		}
//
//		vector normalized_fast() const {
//			vector tmp;
//			tmp.normalize_fast();
//			return tmp;
//		}
//
//		vector operator+(
//			const vector& other) const
//		{
//			vector total;
//			for (size_t i = 0; i < _count; i++) {
//				total.components[i] = components[i] + other.components[i];
//			}
//			return total;
//		}
//
//		vector operator-(
//			const vector& other) const
//		{
//			vector total;
//			for (size_t i = 0; i < _count; i++) {
//				total.components[i] = components[i] - other.components[i];
//			}
//			return total;
//		}
//
//		vector operator*(
//			const vector& other) const
//		{
//			vector total;
//			for (size_t i = 0; i < _count; i++) {
//				total.components[i] = components[i] * other.components[i];
//			}
//			return total;
//		}
//
//		vector operator/(
//			const vector& other) const
//		{
//			vector total;
//			for (size_t i = 0; i < _count; i++) {
//				total.components[i] = components[i] / other.components[i];
//			}
//			return total;
//		}
//
//		vector& operator+=(const vector& other) { return *this = (*this) + other; }
//		vector& operator-=(const vector& other) { return *this = (*this) - other; }
//		vector& operator*=(const vector& other) { return *this = (*this) * other; }
//		vector& operator/=(const vector& other) { return *this = (*this) / other; }
//
//		vector operator-() const {
//			for (_t& c : components) {
//				c = -c;
//			}
//		}
//
//		_t& operator[](
//			size_t index)
//		{
//			return components[index]; // TODO: unsafe
//		}
//
//		const _t& operator[](
//			size_t index) const
//		{
//			return components[index]; // TODO: unsafe
//		}
//
//		bool operator==(
//			const vector& other) const
//		{
//			for (size_t i = 0; i < _count; i++) {
//				if (components[i] != other.components[i]) {
//					return false;
//				}
//			}
//			return true;
//		}
//
//		bool operator!=(
//			const vector& other) const
//		{
//			return !operator==(other);
//		}
//
//		friend vector operator*(
//			_t left,
//			const vector& right)
//		{
//			return right * left;
//		}
//
//		//template<
//		//	typename = std::enable_if<_count == 3>>
//		//friend vector<3> operator*(
//		//	quaternion& q,
//		//	const vector<3>& v)
//		//{
//		//	vector<3> u(q.x, q.y, q.z);
//		//	float s = q.w;
//
//		//	return 2.0f * dot(u, v) * u
//		//		+ (s * s - dot(u, u)) * v
//		//		+ 2.0f * s * cross(u, v);
//		//}
//
//		friend std::ostream& operator<<(
//			std::ostream& ostream,
//			const vector& vector)
//		{
//			ostream << "(";
//			for (size_t i = 0; i < _count; i++) {
//				ostream << vector.components[i];
//				if (i < _count - 1) {
//					ostream << " ";
//				}
//			}
//			return ostream << ")";
//		}
//	};
//}
//
//using namespace math;
//}
