#pragma once

#include "iwmath.h"
#include "vector3.h"
#include "vector4.h"
#include <ostream>

#include "vector.h"

namespace iw {
namespace math {
	struct REFLECT IWMATH_API quaternion {
		static const quaternion identity;

		REFLECT float x;
		REFLECT float y;
		REFLECT float z;
		REFLECT float w;

		quaternion();
		quaternion(float x, float y, float z, float w);
		quaternion(const vector3& xyz, float w);
		quaternion(const vector4& xyzw);

		float length() const;
		float length_squared() const;
		float length_fast() const;

		quaternion normalized() const;
		quaternion normalized_fast() const;
		quaternion inverted() const;

		void normalize();
		void normalize_fast();
		void invert();

		vector4 axis_angle() const;
		vector3 euler_angles() const;

		quaternion operator+(const quaternion& other) const;
		quaternion operator-(const quaternion& other) const;
		quaternion operator*(const quaternion& other) const;
		quaternion operator*(      float       other) const;

		quaternion& operator+=(const quaternion& other);
		quaternion& operator-=(const quaternion& other);
		quaternion& operator*=(const quaternion& other);
		quaternion& operator*=(      float       other);

		bool operator==(const quaternion& other) const;
		bool operator!=(const quaternion& other) const;

		static quaternion from_axis_angle(const vector4& axisAngle);
		static quaternion from_axis_angle(const vector3& axis, float angle);
		static quaternion from_axis_angle(float x, float y, float z, float angle);

		static quaternion from_euler_angles(const vector3& angles);
		static quaternion from_euler_angles(float x, float y, float z);

		static quaternion from_look_at(
			vector3 eye,
			vector3 target = vector3::zero,
			vector3 up     = vector3::unit_y);

		friend std::ostream& operator<<(
			std::ostream& stream,
			const quaternion& a);
	};
}

	using namespace math;
}
