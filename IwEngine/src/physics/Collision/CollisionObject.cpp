#include "iw/physics/Collision/CollisionObject.h"

namespace IW {
	Physics::CollisionObject::CollisionObject()
		: m_transform(nullptr)
		, m_collider(nullptr)
		, m_id(-1)
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

	size_t CollisionObject::Id() const {
		return m_id;
	}

	void CollisionObject::SetId(
		size_t id)
	{
		m_id = id;
	}

}
