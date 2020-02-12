#pragma once

#include "Solver.h"
#include "iw/events/callback.h"
#include "iw/util/memory/ref.h"

// https://www.youtube.com/watch?v=1RphLzpQiJY  Debugging like this could be really cool
// https://www.youtube.com/watch?v=SHinxAhv1ZE  I now understand the goal of this is to have basically a bunch of these constraint rules be applied on contact points
// https://www.youtube.com/watch?v=7_nKOET6zwI  Continuous Collisions      49:00 for the $

namespace iw {
namespace Physics {
	class CollisionSpace {
	public:
		using CollisionCallback = iw::callback<Manifold&, scalar>;
	protected:
		CollisionCallback m_collisionCallback;
	private:
		std::vector<CollisionObject*> m_objects;
		std::vector<Solver*> m_solvers;

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

		IWPHYSICS_API
		void SetCollisionCallback(
			const CollisionCallback& callback);
	protected:
		IWPHYSICS_API
		virtual void SolveManifolds(
			std::vector<Manifold>& manifolds,
			scalar dt = 0);

		IWPHYSICS_API
		virtual void SendCollisionCallbacks(
			std::vector<Manifold>& manifolds,
			scalar dt = 0);
	};
}

	using namespace Physics;
}
