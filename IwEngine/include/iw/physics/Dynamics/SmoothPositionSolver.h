#pragma once

#include "iw/physics/Collision/Solver.h"

namespace iw {
namespace Physics {
	class SmoothPositionSolver
		: public Solver
	{
	public:
		IWPHYSICS_API
		void Solve(
			std::vector<Manifold>& manifolds,
			scalar dt) override;
	};
}

	using namespace Physics;
}
