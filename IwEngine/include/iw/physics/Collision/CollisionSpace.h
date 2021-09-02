#pragma once

#include "Solver.h"
//#include "iw/physics/Ray.h"
#include "iw/util/memory/ref.h"
#include "iw/util/thread/thread_pool.h"

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

		iw::ref<iw::thread_pool> m_task;

	public:
		// this is for testing the swapping of manifold points, the order of objects SHOULD NOT matter
		inline void debug__ScrambleObjects()
		{
			std::vector<CollisionObject*> objects;
			while (m_objects.size() > 0)
			{
				auto itr = m_objects.begin() + iw::randi(int(m_objects.size()) - 1);
				objects.push_back(*itr);
				m_objects.erase(itr);
			}

			m_objects = objects;
		}

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
			Collider& collider) const;

		IWPHYSICS_API
		bool TestObject(
			CollisionObject* object) const;

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
		void SetMultithread(
			iw::ref<iw::thread_pool> task);

		IWPHYSICS_API
		const std::vector<CollisionObject*>& CollisionObjects() const;
	protected:
		void SolveManifolds(
			std::vector<Manifold>& manifolds,
			scalar dt = 0);

		void SendCollisionCallbacks(
			std::vector<Manifold>& manifolds,
			scalar dt = 0);
	};
}

	using namespace Physics;
}
