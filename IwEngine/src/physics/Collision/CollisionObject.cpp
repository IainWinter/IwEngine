#include "iw/physics/Collision/CollisionObject.h"

namespace iw {
namespace Physics {
	CollisionObject::CollisionObject()
		: m_transform()
		, m_collider(nullptr)
		, m_isTrigger(false)
		, m_isDynamic(false)
	{}

	AABB CollisionObject::Bounds() const {
		AABB bounds = Col()->Bounds();
		bounds.Min += Trans().Position;
		bounds.Max += Trans().Position;

		return bounds;
	}

	Transform& CollisionObject::Trans() {
		return m_transform;
	}

	const Transform& CollisionObject::Trans() const {
		return m_transform;
	}

	Transform CollisionObject::ColTrans() const {
		const Transform& tran = Trans();
		      Transform  coll = m_collider->Trans();

		coll.Position += tran.Position;
		coll.Scale    *= tran.Scale;
		coll.Rotation *= tran.Rotation;

		return coll;
	}

	Collider* CollisionObject::Col() const {
		return m_collider;
	}

	bool CollisionObject::IsDynamic() const {
		return m_isDynamic;
	}

	bool CollisionObject::IsTrigger() const {
		return m_isTrigger;
	}

	void CollisionObject::SetTrans(
		Transform* transform)
	{
		m_transform = *transform;
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
}
