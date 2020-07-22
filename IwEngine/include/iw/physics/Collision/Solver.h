#pragma once

#include "CollisionObject.h"
#include "Manifold.h"
#include <vector>

namespace iw {
namespace Physics {
	class Solver {
	public:
		virtual void Solve(
			std::vector<Manifold>& manifolds,
			scalar dt) = 0;
	};
}

	using namespace Physics;
}
