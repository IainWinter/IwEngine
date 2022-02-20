#include "Systems/KeepInWorld_System.h"

void KeepInWorldSystem::FixedUpdate()
{
	//Space->Query<KeepInWorld, iw::Rigidbody>().Each(
	//	[&](
	//		iw::EntityHandle, 
	//		KeepInWorld*, 
	//		iw::Rigidbody* rigidbody) 
	//	{
	//		float x = rigidbody->Transform.Position.x;
	//		float y = rigidbody->Transform.Position.y;

	//		bool atTop    = y > borderFar;
	//		bool atBottom = y < borderNear;
	//		bool atRight  = x > borderFar;
	//		bool atLeft   = x < borderNear;

	//		float& dx = rigidbody->Velocity.x;
	//		float& dy = rigidbody->Velocity.y;

	//		if (atRight  && dx > 0) dx = 0;
	//		if (atLeft   && dx < 0) dx = 0;
	//		if (atTop    && dy > 0) dy = 0;
	//		if (atBottom && dy < 0) dy = 0;

	//		float distOutsideTop    = y - borderFar  - borderMargin;
	//		float distOutsideBottom = y - borderNear + borderMargin;
	//		float distOutsideRight  = x - borderFar  - borderMargin;
	//		float distOutsideLeft   = x - borderNear + borderMargin;

	//		if (distOutsideTop    > 0)  dy -= 10 * distOutsideTop;
	//		if (distOutsideBottom < 0 ) dy -= 10 * distOutsideBottom;
	//		if (distOutsideRight  > 0 ) dx -= 10 * distOutsideRight;
	//		if (distOutsideLeft   < 0 ) dx -= 10 * distOutsideLeft;
	//	});

	for (auto [rigidbody] : entities().query<KeepInWorld, iw::Rigidbody>().only<iw::Rigidbody>())
	{
		float x = rigidbody.Transform.Position.x;
		float y = rigidbody.Transform.Position.y;
		
		bool atTop    = y > borderFar;
		bool atBottom = y < borderNear;
		bool atRight  = x > borderFar;
		bool atLeft   = x < borderNear;
		
		float& dx = rigidbody.Velocity.x;
		float& dy = rigidbody.Velocity.y;
		
		if (atRight  && dx > 0) dx = 0;
		if (atLeft   && dx < 0) dx = 0;
		if (atTop    && dy > 0) dy = 0;
		if (atBottom && dy < 0) dy = 0;
		
		float distOutsideTop    = y - borderFar  - borderMargin;
		float distOutsideBottom = y - borderNear + borderMargin;
		float distOutsideRight  = x - borderFar  - borderMargin;
		float distOutsideLeft   = x - borderNear + borderMargin;
		
		if (distOutsideTop    > 0)  dy -= 10 * distOutsideTop;
		if (distOutsideBottom < 0 ) dy -= 10 * distOutsideBottom;
		if (distOutsideRight  > 0 ) dx -= 10 * distOutsideRight;
		if (distOutsideLeft   < 0 ) dx -= 10 * distOutsideLeft;
	}
}
