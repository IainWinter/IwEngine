#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/physics/Dynamics/Rigidbody.h"

namespace iw {
namespace Physics {
	void SmoothPositionSolver::Solve(
		std::vector<CollisionObject*>& bodies,
		std::vector<Manifold>& manifolds)
	{
		for (Manifold& manifold : manifolds) {
			Rigidbody* aBody = manifold.ObjA->IsDynamic() ? (Rigidbody*)manifold.ObjA : nullptr;
			Rigidbody* bBody = manifold.ObjB->IsDynamic() ? (Rigidbody*)manifold.ObjB : nullptr;

			scalar aInvMass = aBody ? aBody->InvMass() : 0.0f;
			scalar bInvMass = bBody ? bBody->InvMass() : 0.0f;

			iw::vector3 resolution = manifold.B - manifold.A;

			const float percent = 0.8f;
			const float slop = 0.01f;

			iw::vector3 correction = manifold.Normal * percent
				* fmax(resolution.length() - slop, 0.0f)
				/ (aInvMass + bInvMass);
		
			if (aBody ? aBody->IsKinematic() : false) {
				aBody->Trans()->Position -= aInvMass * correction;
			}

			if (bBody ? bBody->IsKinematic() : false) {
				bBody->Trans()->Position += bInvMass * correction;
			}
		}
	}
}
}
