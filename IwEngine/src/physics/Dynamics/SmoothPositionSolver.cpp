#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/physics/Dynamics/Rigidbody.h"

#include "iw/log/logger.h"

namespace iw {
namespace Physics {
	void SmoothPositionSolver::Solve(
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		std::vector<std::pair<iw::vector3, iw::vector3>> deltas;

		for (Manifold& manifold : manifolds) {
			Rigidbody* aBody = manifold.ObjA->IsDynamic() ? (Rigidbody*)manifold.ObjA : nullptr;
			Rigidbody* bBody = manifold.ObjB->IsDynamic() ? (Rigidbody*)manifold.ObjB : nullptr;

			scalar aInvMass = aBody ? aBody->InvMass : 0.0f;
			scalar bInvMass = bBody ? bBody->InvMass : 0.0f;

			const float percent = 0.8f;
			const float slop = 0.01f;

			iw::vector3 correction = manifold.Normal * percent
				* fmax(manifold.PenetrationDepth - slop, 0.0f)
				/ (aInvMass + bInvMass);
		
			iw::vector3 deltaA;
			iw::vector3 deltaB;

			if (aBody ? aBody->IsKinematic : false) {
				deltaA = -aInvMass * correction;
			}

			if (bBody ? bBody->IsKinematic : false) {
				deltaB = bInvMass * correction;
			}

			deltas.push_back(std::make_pair(deltaA, deltaB));
		}

		for (unsigned i = 0; i < manifolds.size(); i++) {
			Rigidbody* aBody = manifolds[i].ObjA->IsDynamic() ? (Rigidbody*)manifolds[i].ObjA : nullptr;
			Rigidbody* bBody = manifolds[i].ObjB->IsDynamic() ? (Rigidbody*)manifolds[i].ObjB : nullptr;

			if (aBody ? aBody->IsKinematic : false) {
				aBody->Trans().Position += deltas[i].first;
			}

			if (bBody ? bBody->IsKinematic : false) {
				bBody->Trans().Position += deltas[i].second;
			}
		}
	}
}
}
