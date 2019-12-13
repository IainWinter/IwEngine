#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/AABB.h"
#include "iw/physics/Ray.h"

namespace IW  {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct Collider {
	protected:
		AABB<V> m_bounds;
		bool m_outdated;

	public:
		//IWPHYSICS_API
		//virtual bool TestCollision(
		//	BoxCollider<V> other,
		//	V* resolve = nullptr) const = 0;
		//
		//IWPHYSICS_API
		//virtual bool TestCollision(
		//	SphereCollider<V> other,
		//	V* resolve = nullptr) const = 0;
		//
		//IWPHYSICS_API
		//virtual bool TestRay(
		//	Ray<V> ray,
		//	V* poi = nullptr) const = 0;
		//
		//IWPHYSICS_API
		//virtual bool TestCollision(
		//	/*MeshColliderV */) const = 0;

		Collider()
			: m_outdated(true)
		{}

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
//namespace IW {
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
//				//	IW::BoxCollider2 other,
//				//	V* resolve = nullptr) const = 0;
//
//				IWPHYSICS_API
//					virtual bool TestCollision(
//						IW::BoxCollider<V> other,
//						V* resolve = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestCollision(
//				//	IW::BoxCollider4 other,
//				//	V* resolve = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestCollision(
//				//	IW::SphereCollider2 other,
//				//	V* resolve = nullptr) const = 0;
//
//				IWPHYSICS_API
//					virtual bool TestCollision(
//						IW::SphereCollider other,
//						V* resolve = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestCollision(
//				//	IW::SphereCollider4 other,
//				//	V* resolve = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestRay(
//				//	IW::Ray2 ray,
//				//	V* poi = nullptr) const = 0;
//
//				IWPHYSICS_API
//					virtual bool TestRay(
//						IW::Ray ray,
//						V* poi = nullptr) const = 0;
//
//				//IWPHYSICS_API
//				//virtual bool TestRay(
//				//	IW::Ray4 ray,
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
