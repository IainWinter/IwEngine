#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"
#include "iw/physics/AABB.h"
#include "iw/engine/Components/Model.h"

namespace IwEngine {
	class IWENGINE_API PhysicsSystem
		: public System<Transform, IwPhysics::AABB3D>
	{
	public:
		PhysicsSystem(
			IwEntity::Space& space,
			IwGraphics::RenderQueue& renderQueue);

		void Update(
			View& view);
	};
}
