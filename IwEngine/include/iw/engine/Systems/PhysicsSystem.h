#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/physics/Dynamics/Rigidbody.h"

namespace iw {
namespace Engine {
	class PhysicsSystem
		: public System<Transform, Rigidbody>
	{
	public:
		struct Components {
			Transform* Transform;
			Rigidbody* Rigidbody;
		};

		float accumulator;

	public:
		IWENGINE_API
		PhysicsSystem();

		IWENGINE_API
		void Update(
			EntityComponentArray& view);

		IWENGINE_API
		void FixedUpdate(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
