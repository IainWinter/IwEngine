#include "Systems/Flocking_System.h"

void FlockingSystem::FixedUpdate()
{
	Space->Query<iw::Transform, iw::Rigidbody, Flocker>().Each([&](
		iw::EntityHandle e,
		iw::Transform* transform,
		iw::Rigidbody* rigidbody,
		Flocker* flocker)
	{
		if (!flocker->Active) return; // for missiles

		glm::vec3 avgAway(0.f);
		glm::vec3 avgCenter(0.f);
		glm::vec3 avgForward(0.f);
		int count = 0;

		Space->Query<iw::Transform, iw::Rigidbody, Flocker>().Each([&](
			iw::EntityHandle e2,
			iw::Transform* transform2,
			iw::Rigidbody* rigidbody2,
			Flocker* flocker2)
		{
			if (e == e2) return;

			glm::vec3 dif = transform->Position - transform2->Position;
			float distance = glm::length(dif);

			if (distance > 200) return;

			// need two scaling functions that allows center to have more effect at long rage and away more close

			float awayWeight = -distance / 2 + 150;
			float centerWeight = -distance + 200;

			avgAway    += (transform->Position - transform2->Position) / awayWeight;
			avgCenter  += transform2->Position / centerWeight;
			avgForward += rigidbody2->Velocity / distance;
			count += 1;
		});

		glm::vec3 targetVel = glm::vec3(flocker->Target, 0.f) - transform->Position;
		float targetDist = glm::length(targetVel);

		//avgAway += targetVel / targetDist;

		if (count > 0) {
			avgAway    /= count;
			avgCenter  /= count; avgCenter -= transform->Position; // transform to local space
			avgForward /= count;
		}

		auto safeNorm = [](glm::vec3 v) 
		{
			return glm::length2(v) > 0.001
				? glm::normalize(v) 
				: glm::vec3(0);
		};
		
		glm::vec3 flockingVel = (avgAway + avgCenter + avgForward) * flocker->ForceWeight;

		rigidbody->Velocity = iw::lerp(rigidbody->Velocity, flockingVel, iw::FixedTime());

		//// Target force, make own funciton in iwmath, i guess a turn twoards
		glm::vec3 nVel = safeNorm(rigidbody->Velocity);
		glm::vec3 nDir = safeNorm(targetVel);
		glm::vec3 delta = (nDir - nVel) * 1.f/** p->Speed * p->TurnRadius*/;

		rigidbody->Velocity = safeNorm(rigidbody->Velocity + delta) * iw::clamp(targetDist, 0.f, 100.f);
	});
}
