#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/common/Events/EventGroups.h"
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
		Rigidbody* BodyA;
		Rigidbody* BodyB;
		scalar PenetrationDepth;

		CollisionEvent(
			Rigidbody* bodyA,
			Rigidbody* bodyB,
			scalar pen,
			scalar dt)
			: PhysicsEvent(PhysicsEventType::Collision, dt)
			, BodyA(bodyA)
			, BodyB(bodyB)
			, PenetrationDepth(pen)
		{}
	};
}

	using namespace Engine;
}
