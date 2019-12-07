#pragma once

#include "Collider.h"
#include "iw/math/vector3.h"
#include "iw/physics/Collision/CollisionMath/CollisionTests.h"
#include <type_traits>

namespace IW {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct BoxCollider
		: Collider<V>
		, ITransformable<BoxCollider<V>>
	{
	private:
		const size_t PointCount = impl::GetNumPoints<V>() + 1;

	public:
		V Points[PointCount];

		BoxCollider(
			V center,
			float scale)
		{
			// gen points
		}

		bool TestCollision(
			const BoxCollider<V>& other,
			V* resolve = nullptr) const override
		{
			return algo::TestCollision(*this, other, resolve);
		}

		bool TestCollision(
			const SphereCollider<V>& other,
			V* resolve = nullptr) const override
		{
			return algo::TestCollision(*this, other, resolve);
		}

		bool TestRay(
			Ray<V> ray,
			V* poi = nullptr) const override
		{
			return algo::TestRay(*this, ray, poi);
		}

		//void Transform(
		//	const IW::Transform& transform) override;

		inline AABB<V> GetAABB() const override {
			return AABB<V>(/*Center, Scale*/);
		}
	};
}

	using BoxCollider2 = impl::BoxCollider<iw::vector2>;
	using BoxCollider  = impl::BoxCollider<iw::vector3>;
	using BoxCollider4 = impl::BoxCollider<iw::vector4>;
}

	using namespace Physics;
}
