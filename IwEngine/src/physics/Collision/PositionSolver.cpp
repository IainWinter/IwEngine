#include "iw/physics/Collision/PositionSolver.h"

namespace iw {
namespace Physics {
	void PositionSolver::Solve(
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			CollisionObject* aBody = manifold.ObjA;
			CollisionObject* bBody = manifold.ObjB;

			iw::vector3 resolution = manifold.Normal * manifold.PenetrationDepth;

			aBody->Trans().Position -= resolution * (1 - (int)aBody->IsStatic());
			bBody->Trans().Position += resolution * (1 - (int)bBody->IsStatic());
		}
	}
}
}
