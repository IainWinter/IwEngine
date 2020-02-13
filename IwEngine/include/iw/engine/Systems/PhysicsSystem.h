#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/physics/Dynamics/Rigidbody.h"

namespace iw {
namespace Engine {
	class PhysicsSystem
		: public System<Rigidbody>
	{
	public:
		IWENGINE_API
		PhysicsSystem();

		IWENGINE_API
		void Update(
			EntityComponentArray& view);

		IWENGINE_API
		bool On(
			EntityDestroyedEvent& e) override;
	};
}

	using namespace Engine;
}
