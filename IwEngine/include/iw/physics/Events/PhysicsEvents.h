#pragma once

#include "iw/events/event.h"
#include "iw/common/Events/EventGroups.h"
#include "iw/physics/Collision/Manifold.h"

namespace iw {
namespace Engine {
	enum class PhysicsEventType
		: short
	{
		Collision
	};

	struct REFLECT PhysicsEvent
		: event
	{
		scalar DeltaTime;

		PhysicsEvent(
			PhysicsEventType type,
			scalar deltaTime
		)
			: event(val(EventGroup::PHYSICS), val(type))
			, DeltaTime(deltaTime)
		{}
	};

	struct REFLECT CollisionEvent
		: PhysicsEvent
	{
		Manifold Manifold;

		CollisionEvent(
			iw::Manifold& manifold,
			scalar dt
)
			: PhysicsEvent(PhysicsEventType::Collision, dt)
			, Manifold(manifold)
		{}
	};
}

	using namespace Engine;
}
