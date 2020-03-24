#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/physics/Dynamics/Rigidbody.h"

namespace iw {
namespace Engine {
	class LightShadowRenderSystem
		: public System<Transform, Light>
	{
	public:
		struct Components {
			Transform* Transform;
			Light* Light;
		};

	public:
		IWENGINE_API
		LightShadowRenderSystem();

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
