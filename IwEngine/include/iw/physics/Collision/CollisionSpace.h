#pragma once

#include "iw/physics/Collision/CollisionObject.h"
#include "Solver.h"
#include <vector>

// https://www.youtube.com/watch?v=1RphLzpQiJY  Debugging like this could be really cool
// https://www.youtube.com/watch?v=SHinxAhv1ZE  I now understand the goal of this is to have basically a bunch of these constraint rules be applied on contact points
// https://www.youtube.com/watch?v=7_nKOET6zwI  Continuous Collisions      49:00 for the $

namespace IW {
namespace Physics {
	class CollisionSpace {
	private:
		std::vector<CollisionObject*> m_objects;
		std::vector<Solver*> m_solvers;

	protected:
		IWPHYSICS_API
		virtual void SolveManifolds(
			std::vector<Manifold>& manifolds,
			scalar dt = 0);
	public:
		IWPHYSICS_API
		virtual void AddCollisionObject(
			CollisionObject* object);

		IWPHYSICS_API
		virtual void RemoveCollisionObject(
			CollisionObject* object);

		IWPHYSICS_API
		void AddSolver(
			Solver* solver);

		IWPHYSICS_API
		void RemoveSolver(
			Solver* solver);

		//IWPHYSICS_API
		//virtual void SolveConstrains();

		IWPHYSICS_API
		bool TestObject(
			CollisionObject* object);

		IWPHYSICS_API
		bool TestObjects(
			CollisionObject* object,
			CollisionObject* other);
	};
}

	using namespace Physics;
}
