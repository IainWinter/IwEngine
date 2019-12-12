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

	// going to need some sort of result to be returned. I like the idea of a lil iwcallback

	bool CollisionSpace::TestObject(
		CollisionObject* object)
	{
		assert(object);

		bool col = false;
		for (CollisionObject* other : m_objects) {
			if (object == other) continue;

			col |= TestObjects(object, other);

			// Would invoke callback with results if there is a collision
		}

		return col;
	}

	bool CollisionSpace::TestObjects(
		CollisionObject* object, 
		CollisionObject* other)
	{
		assert(object && other);

		return object->Col()->Bounds().Intersects(other->Col()->Bounds()); // non transformed aabb's always collide
	}
}
