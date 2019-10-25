#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"
#include "iw/physics/AABB.h"
#include "iw/engine/Components/Model.h"

namespace IwEngine {
	class IWENGINE_API PhysicsSystem
		: public System<Transform, IwPhysics::AABB2D>
	{
	public:
		PhysicsSystem(
			IwEntity::Space& space,
			IW::Graphics::Renderer& renderer);

		void Update(
			IwEntity::EntityComponentArray& view);
	};
}
