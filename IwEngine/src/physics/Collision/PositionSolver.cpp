#include "iw/physics/Collision/PositionSolver.h"

namespace iw {
namespace Physics {
	void PositionSolver::Solve(
		std::vector<CollisionObject*>& bodies,
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			Rigidbody* aBody = manifold.BodyA;
			Rigidbody* bBody = manifold.BodyB;

			iw::vector3 resolution = manifold.B - manifold.A;

			const float percent = 0.8f;
			const float slop = 0.1f;

			iw::vector3 correction = manifold.Normal * percent
				* fmax(resolution.length() - slop, .0001f)
				/ (aBody->InvMass() + bBody->InvMass());
		
			if (aBody->IsKinematic()) {
				aBody->Trans()->Position -= aBody->InvMass() * correction;
			}

			if (bBody->IsKinematic()) {
				bBody->Trans()->Position += bBody->InvMass() * correction;
			}
		}
	}
}
}
