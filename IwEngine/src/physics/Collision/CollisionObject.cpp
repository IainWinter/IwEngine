#include "iw/physics/Collision/CollisionObject.h"

namespace IW {
	CollisionObject::CollisionObject()
		: m_transform(nullptr)
		, m_collider(nullptr)
		, m_isTrigger(false)
	{}

	AABB CollisionObject::Bounds() const {
		AABB bounds = Col()->Bounds();
		bounds.Min += Trans()->Position;
		bounds.Max += Trans()->Position;

		return bounds;
	}

	Transform* CollisionObject::Trans() const {
		return m_transform;
	}

	Collider* CollisionObject::Col() const {
		return m_collider;
	}

	bool CollisionObject::IsTrigger() const {
		return m_isTrigger;
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
	
	void CollisionObject::SetIsTrigger(
		bool isTrigger)
	{
		m_isTrigger = isTrigger;
	}
}
