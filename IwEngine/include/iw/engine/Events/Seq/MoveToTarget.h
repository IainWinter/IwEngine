#pragma once

#include "iw/events/seq/event_task.h"
#include "iw/engine/Time.h"
#include "iw/entity/Entity.h"
#include "iw/physics/Dynamics/Rigidbody.h"

namespace iw {
namespace Engine {
	struct MoveToTarget
		: event_task
	{
	private:
		Entity& entity; // in future it would be awesome if there is a verison that doesnt need a reference
		vector3 target;
		vector3 origin;
		// add speed here

		float startTime;
		bool firstRun;

	public:
		MoveToTarget(
			Entity& entity,
			vector3 target) // add speed here also
			: entity(entity)
			, target(target)
			, origin(0)
			// set speed value
			, startTime(0)
			, firstRun(true)
		{}

		void reset() override {
			firstRun = true;
		}

		bool update() override {
			Transform* transform = nullptr;

			if (entity.Has<Rigidbody>()) {
				Rigidbody* body = entity.Find<Rigidbody>();
				transform = &body->Trans();
			}

			else if (entity.Has<CollisionObject>()) {
				CollisionObject* object = entity.Find<CollisionObject>();
				transform = &object->Trans();
			}

			else if (entity.Has<Transform>()) {
				transform = entity.Find<Transform>();
			}

			if (!transform) {
				return true;
			}

			if (firstRun) {
				origin    = transform->Position;
				startTime = Time::TotalTime();
				firstRun  = false;
			}

			transform->Position = lerp(origin, target, Time::TotalTime() - startTime); // apply speed here (time * speed) 

			return transform->Position == target;
		}
	};
	
}

	using namespace Engine;
}
