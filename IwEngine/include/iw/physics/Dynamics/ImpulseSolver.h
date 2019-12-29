#pragma once

#include "DynamicsSolver.h"

namespace IW {
namespace Physics {
	class ImpulseSolver
		: public DynamicsSolver
	{
	public:
		IWPHYSICS_API
		void Solve(
			std::vector<Rigidbody*>& bodies,
			std::vector<Manifold>& manifolds,
			scalar dt) override;
	};
}

	using namespace Physics;
}
