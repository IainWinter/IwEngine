#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/Collision/Collider.h"

namespace IW {
namespace Physics {
	class CollisionObject {
	protected:
		Transform* m_transform;     // Position, rotation, and scale
		Collider*  m_collider;      // Shape of the collision object (only 3d right now)

	public:
		Transform* Trans() const;
		Collider* Col() const; // should be const probly also these are not needed if they are pointers but Ill try the get set thigs for now as a lil test because ive always dismissed it for the most part

		IWPHYSICS_API
		void SetTrans(
			Transform* transform);

		IWPHYSICS_API
		void SetCol(
			Collider* collider);

		IWPHYSICS_API
		AABB Bounds() const;
	};
}

	using namespace Physics;
}
