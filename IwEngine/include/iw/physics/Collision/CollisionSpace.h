#pragma once

#include "Solver.h"
//#include "iw/physics/Ray.h"
#include "iw/util/memory/ref.h"

// https://www.youtube.com/watch?v=1RphLzpQiJY  Debugging like this could be really cool
// https://www.youtube.com/watch?v=SHinxAhv1ZE  I now understand the goal of this is to have basically a bunch of these constraint rules be applied on contact points
// https://www.youtube.com/watch?v=7_nKOET6zwI  Continuous Collisions      49:00 for the $

namespace iw {
namespace Physics {
	class CollisionSpace {
	protected:
		func_CollisionCallback m_collisionCallback;
		std::vector<CollisionObject*> m_objects;
	private:
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

		IWPHYSICS_API
		void ResolveConstrains(
			scalar dt = 0);

		IWPHYSICS_API
		bool TestCollider(
			const Collider& collider) const;

		//IWPHYSICS_API
		//bool TestObject(
		//	CollisionObject* object);

		//IWPHYSICS_API
		//bool TestObjects(
		//	CollisionObject* object,
		//	CollisionObject* other);

		//IWPHYSICS_API
		//bool TestRay(
		//	Ray* ray,
		//	RayTestResults* out = nullptr);

		//IWPHYSICS_API
		//bool TestRay(
		//	Ray* ray,
		//	CollisionObject* other);

		IWPHYSICS_API
		void SetCollisionCallback(
			const func_CollisionCallback& callback);

		IWPHYSICS_API
		const std::vector<CollisionObject*>& CollisionObjects() const;
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
