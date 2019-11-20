#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"
#include "iw/physics/AABB.h"
#include "iw/engine/Components/Model.h"

namespace IW {
inline namespace Engine {
	class IWENGINE_API PhysicsSystem
		: public System<IW::Transform, IwPhysics::AABB3D>
	{
	public:
		PhysicsSystem();

		void Update(
			IW::EntityComponentArray& view);
	};
}
}
