#include "iw/physics/Collision/PositionSolver.h"

namespace IW {
	void PositionSolver::Solve(
		std::vector<CollisionObject*>& bodies,
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			Rigidbody* aBody = manifold.BodyA;
			Rigidbody* bBody = manifold.BodyB;

			iw::vector3 resolution = manifold.B - manifold.A;

			//if (resolution == 0.0f) {
			//	resolution = 0.1f; // if objects are exactly ontop of eachother nudge them a little
			//}

			// from old iwengine not sure where this is actualy from

			// wtf is this magic LOL this fixed everything and now i understand what i need to do!!! Let's go!

			const float percent = 0.8f;
			const float slop = 0.1f;
			iw::vector3 correction = manifold.Normal * percent * fmax(resolution.length() - slop, .0001f) / (aBody->InvMass() + bBody->InvMass());
		
			if (aBody->IsKinematic()) {
				aBody->Trans()->Position -= aBody->InvMass() * correction;
			}

			if (bBody->IsKinematic()) {
				bBody->Trans()->Position += bBody->InvMass() * correction;
			}
		}

		//iw::vector3 impulse = -manifold.BodyB->Mass() * (resolution / 2 + (manifold.BodyB->Velocity() - manifold.BodyA->Velocity().length()));
	}
}
