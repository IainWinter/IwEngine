#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/Rigidbody.h"

namespace iw {
namespace Physics {
	void ImpulseSolver::Solve(
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			// Test for is each objects is dynamic or not

			Rigidbody* aBody = manifold.ObjA->IsDynamic() ? (Rigidbody*)manifold.ObjA : nullptr;
			Rigidbody* bBody = manifold.ObjB->IsDynamic() ? (Rigidbody*)manifold.ObjB : nullptr;

			vector3 aVel = aBody ? aBody->Velocity() : 0.0f;
			vector3 bVel = bBody ? bBody->Velocity() : 0.0f;
			vector3 rVel = bVel - aVel;
			scalar  nSpd = rVel.dot(manifold.Normal);

			scalar aInvMass = aBody ? aBody->InvMass() : 0.0f;
			scalar bInvMass = bBody ? bBody->InvMass() : 0.0f;

			// Impluse

			if (nSpd >= 0)
				continue;

			scalar e = (aBody ? aBody->Restitution() : 0.0f)
				     * (bBody ? bBody->Restitution() : 0.0f);

			scalar j = -(1.0f + e) * nSpd / (aInvMass + bInvMass);

			vector3 impluse = j * manifold.Normal;

			if (aBody ? aBody->IsKinematic() : false) {
				aVel -= impluse * aInvMass;
			}

			if (bBody ? bBody->IsKinematic() : false) {
				bVel += impluse * bInvMass;
			}

			// Friction

			rVel = bVel - aVel;
			nSpd = rVel.dot(manifold.Normal);

			iw::vector3 tangent = (rVel - nSpd * manifold.Normal).normalized();
			scalar      fVel = rVel.dot(tangent);

			scalar aSF = aBody ? aBody->StaticFriction()  : 0.0f;
			scalar bSF = bBody ? bBody->StaticFriction()  : 0.0f;
			scalar aDF = aBody ? aBody->DynamicFriction() : 0.0f;
			scalar bDF = bBody ? bBody->DynamicFriction() : 0.0f;
			scalar mu  = iw::vector2(aSF, bSF).length();

			scalar f  = -fVel / (aInvMass + bInvMass);

			vector3 friction;
			if (abs(f) < j * mu) {
				friction = f * tangent;
			}

			else {
				mu = vector2(aDF, bDF).length();
				friction = -j * tangent * mu;
			}

			if (aBody ? aBody->IsKinematic() : false) {
				aBody->SetVelocity(aVel - friction * aInvMass);
			}

			if (bBody ? bBody->IsKinematic() : false) {
				bBody->SetVelocity(bVel + friction * bInvMass);
			}
		}
	}
}
}
