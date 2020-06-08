#pragma once

#include "iw/common/Events/EventGroups.h"
#include "iw/physics/Collision/Manifold.h"
#include "iw/events/event.h"

namespace iw {
namespace Engine {
	enum class PhysicsEventType
		: short
	{
		Collision
	};

	struct PhysicsEvent
		: iw::event
	{
		scalar DeltaTime;

		PhysicsEvent(
			PhysicsEventType type,
			scalar deltaTime)
			: iw::event(iw::val(EventGroup::PHYSICS), iw::val(type))
			, DeltaTime(deltaTime)
		{}
	};

	struct CollisionEvent
		: PhysicsEvent
	{
		Manifold Manifold;

		CollisionEvent(
			iw::Manifold& manifold,
			scalar dt)
			: PhysicsEvent(PhysicsEventType::Collision, dt)
			, Manifold(manifold)
		{}
	};
}

	using namespace Engine;
}
