#include "iw/physics/Collision/MeshCollider.h"
#include "iw/physics/Collision/CapsuleCollider.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/algo/GJK.h"

namespace iw {
namespace Physics {
	template<>
	inline ManifoldPoints MeshCollider::TestCollision(
		const Transform* transform,
		const Physics::SphereCollider* sphere,
		const Transform* sphereTransform) const
	{
		algo::GJK();
	}

	template<>
	ManifoldPoints MeshCollider::TestCollision(
		const Transform* transform,
		const Physics::CapsuleCollider* capsule,
		const Transform* capsuleTransform) const
	{

	}

	template<>
	ManifoldPoints MeshCollider::TestCollision(
		const Transform* transform,
		const Physics::PlaneCollider* plane,
		const Transform* planeTransform) const
	{

	}
}
}
