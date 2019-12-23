#pragma once

#include "Rigidbody.h"
#include "iw/physics/Collision/Solver.h"
#include <vector>

namespace IW {
namespace Physics {
	class RigidbodySolver
		: public Solver
{
	public:
		IWPHYSICS_API
		virtual void Solve(
			std::vector<Rigidbody*>& bodies,
			scalar dt) = 0;
	};
}

	using namespace Physics;
}
