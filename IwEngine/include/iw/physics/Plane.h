#pragma once

#include "IwPhysics.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct REFLECT Plane
	{
		REFLECT V P;
		REFLECT float D;

		Plane(
			V points,
			float d)
			: P(points)
			, D(d)
		{}

		float X() const {
			return -D / P.x;
		}

		float Y() const {
			return -D / P.y;
		}

		float Z() const {
			return -D / P.z;
		}

		float Distance() const {
			return D / P.length();
		}
	};
}

	using Plane2 = impl::Plane<glm::vec2>;
	using Plane  = impl::Plane<glm::vec3>;
	using Plane4 = impl::Plane<glm::vec4>;
}

	using namespace Physics;
}
