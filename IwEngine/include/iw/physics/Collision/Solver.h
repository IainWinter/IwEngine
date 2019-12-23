#pragma once

#include "CollisionObject.h"
#include <vector>

namespace IW {
namespace Physics {
	class Solver {
	public:
		virtual void Solve(
			std::vector<CollisionObject>* objects,
			scalar dt) {}
	};
}

	using namespace Physics;
}
