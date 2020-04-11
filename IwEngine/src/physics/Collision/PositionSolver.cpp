#include "iw/physics/Collision/PositionSolver.h"

namespace iw {
namespace Physics {
	void PositionSolver::Solve(
		std::vector<CollisionObject*>& bodies,
		std::vector<Manifold>& manifolds)
	{
		for (Manifold& manifold : manifolds) {
			CollisionObject* aBody = manifold.ObjA;
			CollisionObject* bBody = manifold.ObjB;

			iw::vector3 resolution = (manifold.B - manifold.A) * 0.5f;
		
			aBody->Trans().Position -= resolution;
			bBody->Trans().Position += resolution;
		}
	}
}
}
