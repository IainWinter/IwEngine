#include "iw/physics/Dynamics/ManifoldSolver.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"

namespace IW {
	void ManifoldSolver::Solve(
		std::vector<Rigidbody*>& bodies)
	{
		std::vector<Manifold> manifolds;
		for (Rigidbody* a : bodies) {
			for (Rigidbody* b : bodies) {
				if (a == b) continue;

				Manifold m = algo::MakeManifold(a, b);
				if (m.HasCollision) {
					manifolds.push_back(m);
				}
			}
		}

		for (Manifold& manifold : manifolds) {
			iw::vector3 resolution = manifold.B - manifold.A;
			manifold.BodyA->Trans()->Position += resolution / 4;
		}
	}
}
