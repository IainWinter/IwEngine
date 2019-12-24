#include "iw/physics/Dynamics/ManifoldSolver.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace IW {
	void ManifoldSolver::Solve(
		std::vector<Rigidbody*>& bodies, 
		scalar dt)
	{
		std::vector<Manifold> manifolds;
		for (Rigidbody* a : bodies) {
			for (Rigidbody* b : bodies) {
				if (a == b) break;

				Manifold m = algo::MakeManifold(a, b);
				if (m.HasCollision) {
					manifolds.push_back(m);
				}
			}
		}

		for (Manifold& manifold : manifolds) {
			Rigidbody* aBody = manifold.BodyA;
			Rigidbody* bBody = manifold.BodyB;

			iw::vector3 resolution = manifold.B - manifold.A;

			if (resolution == 0.0f) {
				resolution = 0.1f; // if objects are exactly ontop of eachother nudge them a little
			}

			iw::vector3 aVel = aBody->Velocity(); // aBody->NextTrans().Position - aBody->Trans()->Position;
			iw::vector3 bVel = bBody->Velocity(); //bBody->NextTrans().Position - bBody->Trans()->Position;
			scalar      rVel = (bVel - aVel).dot(manifold.Normal);

			if (rVel > 0)
				continue;

			scalar e = 1.0f;
			scalar j = -(1.0f + e) * rVel / (aBody->InvMass() + bBody->InvMass());

			iw::vector3 impluse = j * manifold.Normal;
			iw::vector3 aImp = -impluse * aBody->InvMass() / dt;
			iw::vector3 bImp =  impluse * bBody->InvMass() / dt;

			if (bBody->Col()->Shape == ColliderShape::PLANE) {
				aBody->ApplyForce(aImp);

				aBody->Trans()->Position += resolution;
			}

			else {
				resolution /= 2;

				aBody->ApplyForce(aImp / 2);
				bBody->ApplyForce(bImp / 2);

				aBody->Trans()->Position += resolution;
				bBody->Trans()->Position -= resolution;
			}
		}

		//iw::vector3 impulse = -manifold.BodyB->Mass() * (resolution / 2 + (manifold.BodyB->Velocity() - manifold.BodyA->Velocity().length()));
	}
}
