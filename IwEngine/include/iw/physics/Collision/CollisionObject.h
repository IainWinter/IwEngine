#pragma once

#include "iw/common/Components/Transform.h"
#include "iw/physics/Collision/Collider.h"

namespace iw {
namespace Physics {
	class CollisionObject {
	protected:
		Transform m_transform;     // Position, rotation, and scale.
		Collider*  m_collider;      // Shape of the collision object (only 3d right now)
		bool m_isTrigger;
		bool m_isDynamic;

	public:
		IWPHYSICS_API
		CollisionObject();

		IWPHYSICS_API
		AABB Bounds() const;

		IWPHYSICS_API
		Transform* Trans();

		IWPHYSICS_API
		const Transform* Trans() const;

		IWPHYSICS_API
		Collider* Col() const; // should be const probly also these are not needed if they are pointers but Ill try the get set thigs for now as a lil test because ive always dismissed it for the most part

		IWPHYSICS_API
		bool IsDynamic() const; // if the collision object is a rigidbody

		IWPHYSICS_API
		bool IsTrigger() const;

		IWPHYSICS_API
		virtual void SetTrans(
			Transform* transform);

		IWPHYSICS_API
		void SetCol(
			Collider* collider);

		IWPHYSICS_API
		void SetIsTrigger(
			bool isTrigger);
	};
}

	using namespace Physics;
}
