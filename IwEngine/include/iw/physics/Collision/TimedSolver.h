#pragma once

#include "Solver.h"

namespace iw {
namespace Physics {
	class TimedSolver
		: public Solver
{
	public:
		IWPHYSICS_API
		virtual void Solve(
			std::vector<CollisionObject*>& objects,
			std::vector<Manifold>& manifolds,
			scalar dt) = 0;
	};
}

	using namespace Physics;
}
