#include "CollisionObject.h"

namespace IW {
	const Transform& CollisionObject::Trans() const {
		return m_transform;
	}

	void CollisionObject::SetTrans(
		const Transform& transform)
	{
		m_transform = transform;
	}
}
