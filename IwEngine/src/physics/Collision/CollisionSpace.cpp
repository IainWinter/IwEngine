#include "iw/physics/Collision/CollisionSpace.h"
#include <assert.h>

namespace IW {
	void CollisionSpace::AddCollisionObject(
		CollisionObject* object)
	{
		assert(object);
		m_objects.push_back(object);
	}

	void CollisionSpace::RemoveCollisionObject(
		CollisionObject* object)
	{
		assert(object);
		auto itr = std::find(m_objects.begin(), m_objects.end(), object);
		
		if (itr != m_objects.end()) {
			m_objects.erase(itr);
		}
	}

	void CollisionSpace::AddSolver(
		Solver* solver)
	{
		assert(solver);
		m_solvers.push_back(solver);
	}

	void CollisionSpace::RemoveSolver(
		Solver* solver)
	{
		assert(solver);
		auto itr = std::find(m_solvers.begin(), m_solvers.end(), solver);

		assert(itr != m_solvers.end());
		m_solvers.erase(itr);
	}

	void CollisionSpace::SolveManifolds(
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Solver* solver : m_solvers) {
			solver->Solve(m_objects, manifolds, dt);
		}
	}

	void CollisionSpace::SendCollisionCallbacks(
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			m_collisionCallback(manifold, dt);
		}
	}

	// going to need some sort of result to be returned. I like the idea of a lil iwcallback

	bool CollisionSpace::TestObject(
		CollisionObject* object)
	{
		assert(object);

		bool col = false;
		for (CollisionObject* other : m_objects) {
			if (object == other) continue;

			col |= TestObjects(object, other);

			// Would invoke callback with results if there is a collision
		}

		return col;
	}

	bool CollisionSpace::TestObjects(
		CollisionObject* object, 
		CollisionObject* other)
	{
		assert(object && other);

		return object->Bounds().Intersects(other->Bounds());
	}

	void CollisionSpace::SetCollisionCallback(
		const CollisionCallback& callback)
	{
		m_collisionCallback = callback;
	}
}
