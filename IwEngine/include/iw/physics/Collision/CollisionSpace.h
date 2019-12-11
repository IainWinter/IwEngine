#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/common/Components/Transform.h"
#include <vector>


namespace IW {
namespace Physics {
	class CollisionSpace {
	private:
		std::vector<CollisionObject*> m_objects;

	public:
		IWPHYSICS_API
		virtual void AddCollisionObject(
			CollisionObject* object);

		IWPHYSICS_API
		virtual void RemoveCollisionObject(
			CollisionObject* object);
	};
}

	using namespace Physics;
}
