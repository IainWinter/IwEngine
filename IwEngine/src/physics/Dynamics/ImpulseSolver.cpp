#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/Rigidbody.h"

namespace iw {
namespace Physics {
	void ImpulseSolver::Solve(
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			// Replaces non dynamic objects with default values.

			Rigidbody* aBody = manifold.ObjA->IsDynamic ? (Rigidbody*)manifold.ObjA : nullptr;
			Rigidbody* bBody = manifold.ObjB->IsDynamic ? (Rigidbody*)manifold.ObjB : nullptr;

			glm::vec3 aVel = aBody ? aBody->Velocity : glm::vec3(0.0f);
			glm::vec3 bVel = bBody ? bBody->Velocity : glm::vec3(0.0f);
			glm::vec3 rVel = bVel - aVel;
			scalar  nSpd = glm::dot(rVel, manifold.Normal);

			scalar aInvMass = aBody ? aBody->InvMass : 1.0f;
			scalar bInvMass = bBody ? bBody->InvMass : 1.0f;

			// Impulse

			// This is important for convergence
			// a negitive impulse would drive the objects closer together
			if (nSpd >= 0)
				continue;

			scalar e = (aBody ? aBody->Restitution : 1.0f)
				     * (bBody ? bBody->Restitution : 1.0f);

			scalar j = -(1.0f + e) * nSpd / (aInvMass + bInvMass);

			glm::vec3 impluse = j * manifold.Normal;

			if (aBody ? aBody->IsSimulated : false) {
				aVel -= impluse * aInvMass;
			}

			if (bBody ? bBody->IsSimulated : false) {
				bVel += impluse * bInvMass;
			}

			// Friction

			rVel = bVel - aVel;
			nSpd = glm::dot(rVel, manifold.Normal);

			glm::vec3 tangent = rVel - nSpd * manifold.Normal;

			if (glm::length(tangent) > 0.0001f) { // safe normalize
				tangent = glm::normalize(tangent);
			}

			scalar fVel = glm::dot(rVel, tangent);

			scalar aSF = aBody ? aBody->StaticFriction  : 0.0f;
			scalar bSF = bBody ? bBody->StaticFriction  : 0.0f;
			scalar aDF = aBody ? aBody->DynamicFriction : 0.0f;
			scalar bDF = bBody ? bBody->DynamicFriction : 0.0f;
			scalar mu  = (scalar)glm::vec2(aSF, bSF).length();

			scalar f  = -fVel / (aInvMass + bInvMass);

			glm::vec3 friction;
			if (abs(f) < j * mu) {
				friction = f * tangent;
			}

			else {
				mu = glm::length(glm::vec2(aDF, bDF));
				friction = -j * tangent * mu;
			}

			if (aBody ? aBody->IsSimulated : false) {
				aBody->Velocity = aVel - friction * aInvMass;
			}

			if (bBody ? bBody->IsSimulated : false) {
				bBody->Velocity = bVel + friction * bInvMass;
			}
		}
	}
}
}
