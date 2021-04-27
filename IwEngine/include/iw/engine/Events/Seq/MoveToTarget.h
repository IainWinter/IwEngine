#pragma once

#include "EventTask.h"
#include "iw/engine/Time.h"
#include "iw/physics/Dynamics/Rigidbody.h"

#include "glm/gtx/compatibility.hpp"

namespace iw {
namespace Engine {
	struct MoveToTarget
		: EventTask
	{
	private:
		Entity& entity; // plz no ref :(
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
			, origin()
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

				posS = speed / (origin.Position - target.Position).length();
				sclS = speed / (origin.Scale    - target.Scale)   .length();
				rotS = speed / (origin.Rotation - target.Rotation).length(); // xd
			}

			if(pos) transform->Position = glm::lerp(origin.Position, target.Position, (Time::TotalTime() - startTime) * posS);
			if(scl) transform->Scale    = glm::lerp(origin.Scale,    target.Scale,    (Time::TotalTime() - startTime) * sclS);
			if(rot) transform->Rotation = glm::lerp(origin.Rotation, target.Rotation, (Time::TotalTime() - startTime) * rotS);

			bool p = pos ? transform->Position == target.Position : true;
			bool s = scl ? transform->Scale    == target.Scale    : true;
			bool r = rot ? transform->Rotation == target.Rotation : true;

			return p && s && r;
		}
	};
	
}

	using namespace Engine;
}
