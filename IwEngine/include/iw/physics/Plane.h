#pragma once

#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"

namespace IW {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct Plane
	{
		V P;
		float D;

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
			return D / P.length_fast();
		}
	};
}

	using Plane2 = impl::Plane<iw::vector2>;
	using Plane  = impl::Plane<iw::vector3>;
	using Plane4 = impl::Plane<iw::vector4>;
}

	using namespace Physics;
}
