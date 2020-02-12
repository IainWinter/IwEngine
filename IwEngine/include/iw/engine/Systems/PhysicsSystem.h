#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/AABB.h"
#include "iw/graphics/Model.h"

namespace iw {
namespace Engine {
	class PhysicsSystem
		: public System<Transform, iw::Physics::AABB>
	{
	public:
		IWENGINE_API
		PhysicsSystem();

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
