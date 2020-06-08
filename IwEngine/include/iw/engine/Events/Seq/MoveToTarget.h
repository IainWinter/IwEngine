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
		Transform target;
		Transform origin;
		float speed;

		float startTime;
		bool firstRun;

		bool pos, scl, rot;
		float posS, sclS, rotS;

	public:
		MoveToTarget(
			Entity& entity,
			vector3 target,
			float speed = 1.0f,
			bool pos = true,
			bool scl = false,
			bool rot = false) // add speed here also
			: entity(entity)
			, target(target)
			, speed(speed)
			, pos(pos)
			, scl(scl)
			, rot(rot)
			, posS(0)
			, sclS(0)
			, rotS(0)
			, origin(0)
			, startTime(0)
			, firstRun(true)
		{}

		MoveToTarget(
			Entity& entity,
			Transform target,
			float speed = 1.0f,
			bool pos = true,
			bool scl = false,
			bool rot = false)
			: entity(entity)
			, target(target)
			, speed(speed)
			, pos(pos)
			, scl(scl)
			, rot(rot)
			, posS(0)
			, sclS(0)
			, rotS(0)
			, origin(0)
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
				origin    = *transform;
				startTime = Time::TotalTime();
				firstRun  = false;

				posS = (origin.Position - target.Position).length() / speed;
				sclS = (origin.Scale    - target.Scale)   .length() / speed;
				rotS = (origin.Rotation - target.Rotation).length() / speed; // xd
			}

			if(pos) transform->Position = lerp(origin.Position, target.Position, (Time::TotalTime() - startTime) / posS);
			if(scl) transform->Scale    = lerp(origin.Scale,    target.Scale,    (Time::TotalTime() - startTime) / sclS);
			if(rot) transform->Rotation = lerp(origin.Rotation, target.Rotation, (Time::TotalTime() - startTime) / rotS);

			bool p = pos ? transform->Position == target.Position : true;
			bool c = scl ? transform->Scale    == target.Scale    : true;
			bool r = rot ? transform->Rotation == target.Rotation : true;

			return p && c && r;
		}
	};
	
}

	using namespace Engine;
}
