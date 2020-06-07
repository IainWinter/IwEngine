#include "iw/physics/Collision/PositionSolver.h"

namespace iw {
namespace Physics {
	void PositionSolver::Solve(
		std::vector<CollisionObject*>& bodies,
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			CollisionObject* aBody = manifold.ObjA;
			CollisionObject* bBody = manifold.ObjB;

			iw::vector3 resolution = (manifold.B - manifold.A);

			aBody->Trans().Position -= resolution * (1 - (int)aBody->IsStatic());
			bBody->Trans().Position += resolution * (1 - (int)bBody->IsStatic());
		}
	}
}
}
