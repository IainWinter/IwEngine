#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/AABB.h"
#include "iw/engine/Components/Model.h"

namespace IW {
inline namespace Engine {
	class PhysicsSystem
		: public System<Transform, AABB3>
	{
	public:
		IWENGINE_API
		PhysicsSystem();

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}
}
