#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/AABB.h"
#include "iw/common/Components/Transform.h"
#include "ManifoldPoints.h"

#include "iw/log/logger.h" // tmp

namespace iw  {
namespace Physics {

	enum class ColliderType {
		SPHERE,
		CAPSULE,
		CYLINDER,
		PLANE,
		//BOX,
		MESH
	};

namespace impl {
	template<typename V>
	struct SphereCollider;

	template<typename V>
	struct CapsuleCollider;

	template<typename V>
	struct CylinderCollider;

	template<typename V>
	struct PlaneCollider;

	template<typename V>
	struct MeshCollider;

	template<
		typename V>
	struct Collider {
	protected:
		ColliderType m_type; // only used for debug nonsense
		AABB<V> m_bounds;
		bool m_outdated;     // if the bounds are outdated

	public:
		Collider(
			ColliderType type)
			: m_type(type)
			, m_outdated(true)
		{}

		ColliderType Type() const {
			return m_type;
		}

		IWPHYSICS_API
		virtual const AABB<V>& Bounds() = 0;

		IWPHYSICS_API
		virtual Transform Trans() const = 0;

		IWPHYSICS_API
		virtual V FindFurthestPoint(
			const Transform* transform,
			V direction) const //= 0;
		{
			LOG_WARNING << "Tried to find furthest point of a collider that doesn't impl function!";
			return V();
		}
	};
}

	using Collider2 = impl::Collider<glm::vec2>;
	using Collider  = impl::Collider<glm::vec3>;
	using Collider4 = impl::Collider<glm::vec4>;

	using SphereCollider2 = impl::SphereCollider<glm::vec2>;
	using SphereCollider  = impl::SphereCollider<glm::vec3>;
	using SphereCollider4 = impl::SphereCollider<glm::vec4>;

	using CapsuleCollider2 = impl::CapsuleCollider<glm::vec2>;
	using CapsuleCollider  = impl::CapsuleCollider<glm::vec3>;
	using CapsuleCollider4 = impl::CapsuleCollider<glm::vec4>;

	using CylinderCollider2 = impl::CylinderCollider<glm::vec2>;
	using CylinderCollider  = impl::CylinderCollider<glm::vec3>;
	using CylinderCollider4 = impl::CylinderCollider<glm::vec4>;

	using PlaneCollider2 = impl::PlaneCollider<glm::vec2>;
	using PlaneCollider  = impl::PlaneCollider<glm::vec3>;
	using PlaneCollider4 = impl::PlaneCollider<glm::vec4>;

	using MeshCollider2 = impl::MeshCollider<glm::vec2>;
	using MeshCollider  = impl::MeshCollider<glm::vec3>;
	using MeshCollider4 = impl::MeshCollider<glm::vec4>;
}

	using namespace Physics;
}
