#pragma once

#include "Collider.h"
#include "iw/math/vector3.h"
#include <type_traits>

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct BoxCollider
		: Collider<V>
	{
	private:
		const size_t PointCount = impl::GetNumPoints<V>() + 1;

	public:
		V Points[PointCount];

		BoxCollider(
			V center,
			float scale)
			: Collider<V>()
		{
			for (int i = 0; i < PointCount; i++) {
				Points[i] = center + (iw::vector4(1)[i] * scale);
			}
		}

		AABB<V> GetAABB() const override {
			return AABB<V>(/*Center, Scale*/);
		}

		V* Translate(


	};
}

	using BoxCollider2 = impl::BoxCollider<iw::vector2>;
	using BoxCollider  = impl::BoxCollider<iw::vector3>;
	using BoxCollider4 = impl::BoxCollider<iw::vector4>;
}

	using namespace Physics;
}
