#pragma once

#include "Rigidbody.h"
#include "iw/physics/Collision/Solver.h"

namespace IW {
namespace Physics {
	class DynamicsSolver
		: public Solver
{
	public:
		IWPHYSICS_API
		virtual void Solve(
			std::vector<Rigidbody*>& bodies,
			std::vector<Manifold>& manifolds,
			scalar dt) = 0;
	};
}

	using namespace Physics;
}
