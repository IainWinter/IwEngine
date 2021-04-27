#pragma once

#include "IwPhysics.h"
#include "ITransformable.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct Ray
		: ITransformable<Ray<V>>
	{
		V Vector;

		Ray(
			V vector)
			: Vector(vector)
		{}

		Ray(
			V a,
			V b)
			: Vector(b - a)
		{}

		void Transform(
			const iw::Transform& transform) override;

		//inline float Angle() const {
		//	return A.dot(B) / (A.length() * B.length());
		//}

		inline float Length() const {
			return Vector.length();
		}
	};
}

	using Ray2 = impl::Ray<glm::vec2>;
	using Ray  = impl::Ray<glm::vec3>;
	using Ray4 = impl::Ray<glm::vec4>;
}

	using namespace Physics;
}
