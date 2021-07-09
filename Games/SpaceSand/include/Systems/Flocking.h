#pragma once

#include "iw/engine/System.h"
#include "Components/Flocker.h"

class FlockingSystem
	: iw::SystemBase
{
public:
	FlockingSystem() : iw::SystemBase("Flocking") {}

	void FixedUpdate() override
	{
		Space->Query<iw::Transform, iw::Rigidbody, Flocker>().Each([&](
			iw::EntityHandle e,
			iw::Transform* transform,
			iw::Rigidbody* rigidbody,
			Flocker* flocker)
		{
			if (!flocker->Active) return; // for missiles

			glm::vec2 avgAway(0);
			glm::vec2 avgCenter(0);
			glm::vec2 avgForward(0);
			int count = 0;

			Space->Query<iw::Transform, iw::Rigidbody>().Each([&](
				iw::EntityHandle e2,
				iw::Transform* transform2,
				iw::Rigidbody* rigidbody2)
			{
				if (e == e2) return;

				avgAway    += transform->Position - transform2->Position;
				avgCenter  += transform2->Position;
				avgForward += rigidbody2->Velocity;
				count      += 1;
			});

 			if (count > 0) {
				avgAway    /= count;
				avgCenter  /= count; avgCenter -= transform->Position; // transform to local space
				avgForward /= count;
			}

			rigidbody->Velocity = glm::vec3(avgAway + avgForward + avgCenter, 0); // incorrect research how to apply this force correctly, maybe make the movement system that everything has to act through

			//avgCenter  = glm::vec2(0); // only using away force for now
			//avgForward = glm::vec2(0);

			//dd = iw::clamp<glm::vec2>(dd, -100, 100); // Flocking force, should cap acceleration?

			//rigidbody->Velocity = iw::lerp(rigidbody->Velocity, rigidbody->Velocity + glm::vec3(dd, 0), iw::FixedTime());

			// Target force, make own funciton in iwmath, i guess a turn twoards
			//glm::vec2 nVel = glm::normalize(rigidbody->Velocity);
			//glm::vec2 nDir = (rigidbody->Target - t->Position).normalized();
			//glm::vec2 delta = (nDir - nVel) * p->Speed * p->TurnRadius;

			//p->Velocity = (p->Velocity + delta).normalized() * p->Speed;
		});
	}
};
