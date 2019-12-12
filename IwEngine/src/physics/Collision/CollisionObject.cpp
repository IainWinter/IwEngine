#include "iw/physics/Collision/CollisionObject.h"

namespace IW {
	Transform* CollisionObject::Trans() const {
		return m_transform;
	}

	Collider* CollisionObject::Col() const {
		return m_collider;
	}

	void CollisionObject::SetTrans(
		Transform* transform)
	{
		m_transform = transform;
	}

	void CollisionObject::SetCol(
		Collider* collider)
	{
		m_collider = collider;
	}
}
