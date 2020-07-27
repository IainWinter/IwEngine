#include "iw/physics/Collision/CollisionSpace.h"
#include "iw/log/logger.h"
#include <assert.h>

namespace iw {
namespace Physics {
	void CollisionSpace::AddCollisionObject(
		CollisionObject* object)
	{
		if (object == nullptr) {
			LOG_WARNING << "Tried to add null object to collision space";
			return;
		}

		m_objects.push_back(object);
	}

	void CollisionSpace::RemoveCollisionObject(
		CollisionObject* object)
	{
		if (object == nullptr) {
			LOG_WARNING << "Tried to remove null object from collision space";
			return;
		}

		auto itr = std::find(m_objects.begin(), m_objects.end(), object);
		
		if (itr == m_objects.end()) {
			LOG_WARNING << "Tried to remove object that doesn't exist in the collision space";
			return;
		}

		m_objects.erase(itr);
	}

	void CollisionSpace::AddSolver(
		Solver* solver)
	{
		if (solver == nullptr) {
			LOG_WARNING << "Tried to add null solver to collision space";
			return;
		}

		m_solvers.push_back(solver);
	}

	void CollisionSpace::RemoveSolver(
		Solver* solver)
	{
		if (solver == nullptr) {
			LOG_WARNING << "Tried to remove null solver from dynamics space";
			return;
		}

		auto itr = std::find(m_solvers.begin(), m_solvers.end(), solver);

		if (itr == m_solvers.end()) {
			LOG_WARNING << "Tried to remove solver that doesn't exist in the collision space";
			return;
		}

		m_solvers.erase(itr);
	}

	void CollisionSpace::ResolveConstrains(
		scalar dt)
	{
		std::vector<Manifold> manifolds;
		std::vector<Manifold> triggers;
		for (CollisionObject* a : m_objects) {
			for (CollisionObject* b : m_objects) {
				if (a == b) break;

				if (   a->IsTrigger()
					&& b->IsTrigger()
					&& !(a->IsDynamic() || b->IsDynamic()))
				{
					continue;
				}

				if (   !a->Col()
					|| !b->Col())
				{
					continue;
				}

				ManifoldPoints points = a->Col()->TestCollision(&a->Trans(), b->Col(), &b->Trans());
				if (points.HasCollision) {
					// establish more formal rules for what can collide with what
					if (   a->IsTrigger()
						|| b->IsTrigger())
					{
						triggers.emplace_back(a, b, points);
					}

					else {
						manifolds.emplace_back(a, b, points);
					}
				}
			}
		}

		SolveManifolds(manifolds, dt);

		SendCollisionCallbacks(manifolds, dt);
		SendCollisionCallbacks(triggers, dt);
	}

	void CollisionSpace::SolveManifolds(
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		//for (int i = 0; i < 5; i++) {
			for (Solver* solver : m_solvers) {
				solver->Solve(manifolds, dt);
			}
		//}
	}

	void CollisionSpace::SendCollisionCallbacks(
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		for (Manifold& manifold : manifolds) {
			m_collisionCallback(manifold, dt);

			func_CollisionCallback& a = manifold.ObjA->OnCollision();
			func_CollisionCallback& b = manifold.ObjB->OnCollision();

			if (a) {
				a(manifold, dt);
			}

			if (b) {
				b(manifold, dt);
			}
		}
	}

	// going to need some sort of result to be returned. I like the idea of a lil iwcallback

	//bool CollisionSpace::TestObject(
	//	CollisionObject* object)
	//{
	//	if (object == nullptr) {
	//		LOG_WARNING << "Tried to test null object against collision space";
	//		return false;
	//	}

	//	bool col = false;
	//	for (CollisionObject* other : m_objects) {
	//		if (object == other) continue;

	//		col |= TestObjects(object, other);

	//		// Would invoke callback with results if there is a collision
	//	}

	//	return col;
	//}

	//bool CollisionSpace::TestObjects(
	//	CollisionObject* object, 
	//	CollisionObject* other)
	//{
	//	if (   object == nullptr
	//		|| other  == nullptr)
	//	{
	//		LOG_WARNING << "Tried to test null objects against each other";
	//		return false;
	//	}

	//	return object->Bounds().Intersects(other->Bounds());
	//}

	//bool CollisionSpace::TestRay(
	//	Ray* ray)
	//{
	//	return false;
	//}

	//bool CollisionSpace::TestRay(
	//	Ray* ray,
	//	CollisionObject* other)
	//{
	//	return false;
	//}

	void CollisionSpace::SetCollisionCallback(
		const func_CollisionCallback& callback)
	{
		m_collisionCallback = callback;
	}

	const std::vector<CollisionObject*>& CollisionSpace::CollisionObjects() const {
		return m_objects;
	}
}
}
