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
		CollisionObject* ObjA;
		CollisionObject* ObjB;
		scalar PenetrationDepth;

		CollisionEvent(
			CollisionObject* objA,
			CollisionObject* objB,
			scalar pen,
			scalar dt)
			: PhysicsEvent(PhysicsEventType::Collision, dt)
			, ObjA(objA)
			, ObjB(objB)
			, PenetrationDepth(pen)
		{}
	};
}

	using namespace Engine;
}
