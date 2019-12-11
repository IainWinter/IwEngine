#include "iw/physics/Collision/CollisionSpace.h"
#include <assert.h>

namespace IW {
	void CollisionSpace::AddCollisionObject(
		CollisionObject* object)
	{
		assert(object);
		m_objects.push_back(object);
	}

	void CollisionSpace::RemoveCollisionObject(
		CollisionObject* object)
	{
		assert(object);
		auto itr = std::find(m_objects.begin(), m_objects.end(), object);
		
		assert(itr != m_objects.end());
		m_objects.erase(itr);
	}
}
