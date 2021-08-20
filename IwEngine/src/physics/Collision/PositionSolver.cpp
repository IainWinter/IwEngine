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

			iw::_vec<d3> resolution = manifold.Normal * manifold.PenetrationDepth / scalar(2);

			aBody->Transform.Position -= resolution * scalar(1 - (int)aBody->IsStatic);
			bBody->Transform.Position += resolution * scalar(1 - (int)bBody->IsStatic);
		}
	}
}
}
