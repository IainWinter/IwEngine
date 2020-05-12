#pragma once
#include "iw/events/seq/event_seq.h"
#include "iw/entity/Entity.h"
#include "iw/math/vector3.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/engine/Time.h"

namespace iw {
namespace Engine {
	struct MoveToTarget:event_seq_item {
		Entity& entity;
		vector3 target;
		vector3 origin;
		bool firstRun;
		float startTime;

		MoveToTarget(
			Entity& entity,
			vector3 target)
			: entity(entity)
			, target(target)
			, origin(0)
			, firstRun(true)
			, startTime(0)
		{}

		bool update() override {
			Rigidbody* body = entity.Find<Rigidbody>();
			Transform transform = body->Trans();
			if (firstRun) {
				firstRun = false;
				origin = transform.Position;
				startTime = Time::TotalTime();
			}

			transform.Position = lerp(origin, target, Time::TotalTime() - startTime);
			body->SetTrans(&transform);

			return transform.Position == target;
		}
	};
	
}

	using namespace Engine;
}
