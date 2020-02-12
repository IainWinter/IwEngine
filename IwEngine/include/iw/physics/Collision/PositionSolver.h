#pragma once

#include "Solver.h"

namespace iw {
namespace Physics {
	class PositionSolver
		: public Solver
	{
	public:
		IWPHYSICS_API
		void Solve(
			std::vector<CollisionObject*>& objects,
			std::vector<Manifold>& manifolds,
			scalar dt) override;
	};
}

	using namespace Physics;
}
