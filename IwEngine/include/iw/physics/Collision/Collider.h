#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/AABB.h"
#include "iw/physics/Collision/ManifoldPoints.h"
#include "iw/common/Components/Transform.h"

namespace iw  {
namespace Physics {
namespace impl {
	template<typename V>
	struct SphereCollider;

	template<typename V>
	struct PlaneCollider;

	template<
		typename V>
	struct Collider {
	protected:
		AABB<V> m_bounds;
		bool m_outdated;

	public:
		Collider()
			: m_outdated(true)
		{}

		// dont like that you need to pass transforms

		IWPHYSICS_API
		virtual ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::Collider<V>* collider,
			const Transform* colliderTransform) const = 0;

		IWPHYSICS_API
		virtual ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::SphereCollider<V>* sphere,
			const Transform* sphereTransform) const = 0;

		IWPHYSICS_API
		virtual ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::PlaneCollider<V>* plane,
			const Transform* planeTransform) const = 0;

		IWPHYSICS_API
		virtual const AABB<V>& Bounds() = 0;
	};
}

	using Collider2 = impl::Collider<iw::vector2>;
	using Collider  = impl::Collider<iw::vector3>;
	using Collider4 = impl::Collider<iw::vector4>;
}

	using namespace Physics;
}


//#pragma once
//
//#include "iw/physics/IwPhysics.h"
//#include "iw/physics/ITransformable.h"
//#include "iw/physics/AABB.h"
//#include "iw/physics/Ray.h"
//
//namespace iw {
//	namespace Physics {
//		namespace impl {
//			template<
//				typename V>
//				struct Collider {
//				AABB<V> Bounds;
//				bool Outdated;
//
//				//IWPHYSICS_API
//				//virtual bool TestCollision(
//				//	iw::BoxCollider2 other,
//				//	V* resolve = nullptr) const = 0;
//
//				IWPHYSICS_API
//					virtual bool TestCollision(
//						iw::BoxCollider<V> other,
//						V* resolve = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestCollision(
//				//	iw::BoxCollider4 other,
//				//	V* resolve = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestCollision(
//				//	iw::SphereCollider2 other,
//				//	V* resolve = nullptr) const = 0;
//
//				IWPHYSICS_API
//					virtual bool TestCollision(
//						iw::SphereCollider other,
//						V* resolve = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestCollision(
//				//	iw::SphereCollider4 other,
//				//	V* resolve = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestRay(
//				//	iw::Ray2 ray,
//				//	V* poi = nullptr) const = 0;
//
//				IWPHYSICS_API
//					virtual bool TestRay(
//						iw::Ray ray,
//						V* poi = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestRay(
//				//	iw::Ray4 ray,
//				//	V* poi = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestCollision(
//				//	/*MeshColliderV */) const = 0;
//
//				IWPHYSICS_API
//					virtual AABB<V> GetAABB() const = 0;
//			};
//		}
//
//		using Collider2 = impl::Collider<iw::vector2>;
//		using Collider = impl::Collider<iw::vector3>;
//		using Collider4 = impl::Collider<iw::vector4>;
//	}
//
//	using namespace Physics;
//}
