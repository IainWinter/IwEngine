#pragma once

#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"

namespace IW {
inline namespace Physics {
	template<
		typename V>
	struct Ray {
		V A;
		V B;

		Ray(
			V a,
			V b)
			: A(a)
			, B(b)
		{}

		inline float Angle() const {
			return A.dot(B) / (A.length() * B.length());
		}

		inline float Length() const {
			return Vector().length();
		}

		inline V Vector() const {
			return B - A;
		}

		// should return int like Intersects
		bool HasPoint(
			V p) const
		{
			float pa = (p - A).length();
			float pb = (p - B).length();
			float ab = (B - A).length();

			return iw::almost_equal(pa + pb, ab, 5);
		}

		//http://geomalgorithms.com/a05-_intersect-1.html#intersect2D_2Segments


		//  0: Not intersecting
		//  1: Same point
		//  2: Intersecting (infront of  B)
		//  3: Intersecting (behind  of  A)
		//  4: Intersecting (inside  of AB)
		virtual int Intersects(
			const Ray<V>& ray,
			V* intersect    = nullptr,
			V* intersectEnd = nullptr) const;
	};

	using Ray2 = Ray<iw::vector2>;
	using Ray3 = Ray<iw::vector3>;
	using Ray4 = Ray<iw::vector4>;

	template<typename V>
	inline int Ray<V>::Intersects(
		const Ray& ray,
		V* intersect,
		V* intersectEnd) const
	{
		assert(false, "Invalid vector for segment");
		return 0;
	}

	//  0: Not intersecting
	//  1: Same point
	//  2: Intersecting (behind  of  A)       l
	//  3: Intersecting (infront of  B)    r  l
	//  4: Intersecting (inside  of AB) s  r  l
	template<>
	inline int Ray<iw::vector2>::Intersects(
		const Ray& ray,
		iw::vector2* intersect,
		iw::vector2* intersectEnd) const
	{
		iw::vector2 u = Vector();
		iw::vector2 v = ray.Vector();
		iw::vector2 w = A - ray.A;
		float d = u.cross_length(v); // only vec2

		iw::vector2 i, ie;
		int o = 0;

		// parallel
		if (iw::almost_equal(d, 0, 5)) {
			if (u.cross_length(w) == 0 || v.cross_length(w) == 0) {
				float du = u.dot(u);
				float dv = v.dot(v);

				// both segments are points
				if (du == 0 && dv == 0) {
					// Same point
					if (A == ray.A) {
						i = A;
						o = 1; // same point
					}
				}

				// 'this' is a point
				else if (du == 0) {
					if (ray.HasPoint(A)) {
						i = A;
						o = -324; // inside of AB WRONG CHECK DISTANCE
					}
				}

				// 'segment' is a point
				else if (dv == 0) {
					if (HasPoint(ray.A)) {
						i = ray.A;
						o = -234; // inside of AB WRONG CHECK DISTANCE
					}
				}

				// collinear
				else {
					float t0, t1;
					iw::vector2 w2 = ray.B - A;
					if (v.x != 0) {
						t0 = w.x / v.x;
						t1 = w2.x / v.x;
					}

					else {
						t0 = w.y / v.y;
						t1 = w2.y / v.y;
					}

					// swap if t0 is larger than t1
					if (t0 > t1) {
						float tmp = t0;
						t0 = t1;
						t1 = tmp;
					}

					// overlap
					if (t0 < 1 || t1 > 0) {
						t0 = t0 < 0 ? 0 : t0;
						t1 = t1 > 1 ? 1 : t1;

						// intersect is a point
						if (t0 == t1) {
							i = ray.A + t0 * v;
						}

						// intersect is a segment
						else {
							i = ray.A + t0 * v;
							i = ray.A + t1 * v;
						}
					}
				}
			}
		}

		// non parallel
		else {
			float si = v.cross_length(w) / d;

			if      (si < 0)           { o = 2; } // behind  of A
			else if (si > 1)           { o = 3; } // infront of B
			else if (si < 1 && si > 0) { o = 4; } // inside of AB

			i = A + si * u;
		}

		if (intersect) {
			*intersect = i;
		}

		if (intersectEnd) {
			*intersectEnd = ie;
		}

		return o;
	}
}
}
