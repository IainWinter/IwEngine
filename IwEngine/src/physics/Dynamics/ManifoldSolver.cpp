#include "iw/physics/Dynamics/ManifoldSolver.h"

namespace IW {
	void ManifoldSolver::Solve(
		std::vector<Rigidbody*>& bodies,
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			Rigidbody* aBody = manifold.BodyA;
			Rigidbody* bBody = manifold.BodyB;

			iw::vector3 aVel = aBody->Velocity(); // aBody->NextTrans().Position - aBody->Trans()->Position;
			iw::vector3 bVel = bBody->Velocity(); //bBody->NextTrans().Position - bBody->Trans()->Position;
			iw::vector3 rVel = bVel - aVel;   
			scalar      nVel = rVel.dot(manifold.Normal);

			//if (nVel > 0)
			//	continue;

			scalar e = 1.0f;
			scalar u = 0.1f;

			scalar j = -(1.0f + e) * nVel / (aBody->InvMass() + bBody->InvMass());

			iw::vector3 impluse = j * manifold.Normal;
			iw::vector3 aImp = -impluse * aBody->InvMass();
			iw::vector3 bImp =  impluse * bBody->InvMass();

			iw::vector3 friction = u * j * rVel.normalized();

			if (aBody->IsKinematic()) {
				aBody->SetVelocity(aBody->Velocity() + aImp + friction * aBody->Mass() * dt);
			}

			if (bBody->IsKinematic()) {
				bBody->SetVelocity(bBody->Velocity() + bImp);
			}
		}

		//iw::vector3 impulse = -manifold.BodyB->Mass() * (resolution / 2 + (manifold.BodyB->Velocity() - manifold.BodyA->Velocity().length()));
	}
}
