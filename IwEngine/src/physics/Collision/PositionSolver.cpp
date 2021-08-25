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

			int aStatic = (int)aBody->IsStatic;
			int bStatic = (int)bBody->IsStatic;

			iw::_vec<d3> resolution = manifold.Normal 
				* manifold.PenetrationDepth 
				/ iw::max<scalar>(1, aStatic + bStatic);

			aBody->Transform.Position -= resolution * scalar(1 - aStatic);
			bBody->Transform.Position += resolution * scalar(1 - bStatic);
		}
	}
}
}
