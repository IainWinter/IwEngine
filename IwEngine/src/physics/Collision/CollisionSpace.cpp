#include "iw/physics/Collision/CollisionSpace.h"
#include "iw/physics/Collision/TestCollision.h"
#include "iw/log/logger.h"
#include <assert.h>

#include "iw/physics/Collision/MeshCollider.h" // temp
#include "iw/physics/Collision/SphereCollider.h" // temp

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
		// Update cache

		auto updateCahce = [](
			CollisionObject* obj)
		{
			Collider* collider = obj->Collider;
			if (   collider 
				&& collider->CacheIsOld()) 
			{
				collider->UpdateCache();
			}
		};

		if (m_task) {
			m_task->foreach(m_objects, [&](int index) {
				updateCahce(m_objects.at(index));
			});
		}

		else {
			for (CollisionObject* obj : m_objects) {
				updateCahce(obj);
			}
		}

		// should have a container that keeps track of this
		std::vector<std::pair<CollisionObject*, CollisionObject*>> pairs;

		for (CollisionObject* a : m_objects) 
		{
			if (!a->Collider) {
				continue;
			}

			for (CollisionObject* b : m_objects) {
				if (a == b) break;

				if (   !b->Collider
					|| (a->IsTrigger && b->IsTrigger)
					|| (a->IsStatic  && b->IsStatic))
				{
					continue;
				}

				// temp 'broad' phase 
				if (   a->Collider->Dim == d2
					&& b->Collider->Dim == d2)
				{
					auto& abounds = ((impl::Collider<d2>*)a->Collider)->Bounds();
					auto& bbounds = ((impl::Collider<d2>*)b->Collider)->Bounds();

					if (!abounds.Intersects(&a->Transform, bbounds, &b->Transform))
					{
						continue;
					}
				}

				pairs.emplace_back(a, b);
			}
		}

		std::vector<Manifold> triggers;
		std::vector<Manifold> collisions;
		std::mutex mutexTriggers, mutexCollisions;

		auto findManifold = [&](
			CollisionObject* a, 
			CollisionObject* b) 
		{
			ManifoldPoints points = TestCollision(a->Collider, &a->Transform, b->Collider, &b->Transform);

			if (points.HasCollision) 
			{
				if (   a->IsTrigger
					|| b->IsTrigger) // establish more formal rules for what can collide with what, see next comments
				{
					std::unique_lock lock(mutexTriggers); // locks on single thread too
					triggers.emplace_back(a, b, points);
				}

				else {
					std::unique_lock lock(mutexCollisions);
					collisions.emplace_back(a, b, points);
				}
			}
		};

		if (m_task) {
			m_task->foreach(pairs, [&](size_t i)  {
				findManifold(pairs[i].first, pairs[i].second);
			});
		}

		else {
			for (auto& [a, b] : pairs) {
				findManifold(a, b);
			}
		}

		SendCollisionCallbacks(collisions, dt);
		SendCollisionCallbacks(triggers, dt);

		for (int i = 0; i < collisions.size(); i++) // in a collision callback they could disable the response
		{
			if (!collisions[i].HasCollision)
			{
				collisions[i] = collisions.back();
				collisions.pop_back();
			}
		}

		SolveManifolds(collisions, dt);
	}


	bool CollisionSpace::TestCollider(
		Collider& collider) const
	{
		for (CollisionObject* other : m_objects) {
			if (   other->Collider
				&& TestCollision(other->Collider, &other->Transform, &collider, &Transform()).HasCollision)
			{
				return true;
			}
		}

		return false;
	}

	bool CollisionSpace::TestObject(
		CollisionObject* object) const
	{
		return TestCollider(*object->Collider);
	}

	DistanceQueryResult CollisionSpace::QueryPoint(
		const glm::vec3& position,
		scalar maxDistance) const
	{
		DistanceQueryResult result;

		for (CollisionObject* object : m_objects)
		{
			// this doesnt take into account collider, should use FindClosestPoint, but even with mesh colliders only returns verts
			// needs ture analyitical closest point type function to be super correct
			// solving this would mean that the exact point from gjk/epa could be found which opens rotational physics finally

			scalar distance = glm::distance2(object->Transform.WorldPosition(), position);
			if (distance < maxDistance * maxDistance)
			{
				result.Objects.emplace(distance, object);
			}
		}
		
		return result;
	}

	DistanceQueryResult CollisionSpace::QueryVector(
		const glm::vec3& position,
		const glm::vec3& vector,
		scalar maxDistance) const
	{
		DistanceQueryResult result;

		glm::vec3 vectorNorm = glm::normalize(vector);

		for (CollisionObject* object : m_objects)
		{
			glm::vec3 dif = object->Transform.WorldPosition() - position;

			if (glm::length2(dif) < maxDistance * maxDistance)
			{
				float distance = glm::dot(vectorNorm, dif);
				if (distance > 0)
				{
					result.Objects.emplace(distance, object);
				}
			}
		}

		return result;
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

			func_CollisionCallback& a = manifold.ObjA->OnCollision;
			func_CollisionCallback& b = manifold.ObjB->OnCollision;

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

	void CollisionSpace::SetMultithread(
		iw::ref<iw::thread_pool> task)
	{
		m_task = task;
	}

	const std::vector<CollisionObject*>& CollisionSpace::CollisionObjects() const {
		return m_objects;
	}
}
}
