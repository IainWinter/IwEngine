#include "Systems/KeepInWorld_System.h"

void KeepInWorldSystem::FixedUpdate()
{
	Space->Query<KeepInWorld, iw::Rigidbody>().Each(
		[&](
			iw::EntityHandle, 
			KeepInWorld*, 
			iw::Rigidbody* rigidbody) 
		{
			bool atTop    = rigidbody->Transform.Position.y > borderFar;
			bool atBottom = rigidbody->Transform.Position.y < borderNear;
			bool atRight  = rigidbody->Transform.Position.x > borderFar;
			bool atLeft   = rigidbody->Transform.Position.x < borderNear;

			float& dx = rigidbody->Velocity.x;
			float& dy = rigidbody->Velocity.y;

			if (atRight  && dx > 0) dx = 0;
			if (atLeft   && dx < 0) dx = 0;
			if (atTop    && dy > 0) dy = 0;
			if (atBottom && dy < 0) dy = 0;

			bool outsideTop    = rigidbody->Transform.Position.y > borderFar  + borderMargin;
			bool outsideBottom = rigidbody->Transform.Position.y < borderNear - borderMargin;
			bool outsideRight  = rigidbody->Transform.Position.x > borderFar  + borderMargin;
			bool outsideLeft   = rigidbody->Transform.Position.x < borderNear - borderMargin;

			if (outsideRight)  dx -= borderMargin;
			if (outsideLeft)   dx += borderMargin;
			if (outsideTop)    dy -= borderMargin;
			if (outsideBottom) dy += borderMargin;
		});
}
