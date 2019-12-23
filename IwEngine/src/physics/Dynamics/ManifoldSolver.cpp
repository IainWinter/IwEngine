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

			iw::vector3 aVel = aBody->NextTrans().Position - aBody->Trans()->Position;
			iw::vector3 bVel = bBody->NextTrans().Position - bBody->Trans()->Position;

			iw::vector3 rVel = (bVel - aVel) * resolution.normalized();

			iw::vector3 aImp =  rVel * manifold.BodyA->Mass() / dt;
			iw::vector3 bImp = -rVel * manifold.BodyB->Mass() / dt;

			if (bBody->Col()->Shape == ColliderShape::PLANE) {
				aBody->ApplyForce(aImp);
				aBody->Trans()->Position += resolution / 2;

				//manifold.BodyA->Trans()->Position += resolution;
			}

			else {
				aBody->ApplyForce(aImp);
				bBody->ApplyForce(bImp);

				aBody->Trans()->Position += resolution / 2;
				bBody->Trans()->Position -= resolution / 2;
				//manifold.BodyA->Trans()->Position += resolution / 2;
			}
		}

		//iw::vector3 impulse = -manifold.BodyB->Mass() * (resolution / 2 + (manifold.BodyB->Velocity() - manifold.BodyA->Velocity().length()));
	}
}
