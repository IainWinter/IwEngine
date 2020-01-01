#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"
#include "iw/log/logger.h"
#include <assert.h>
#include <vector>

namespace IW {
	void DynamicsSpace::SolveManifolds(
		std::vector<Manifold>& manifolds,
		scalar dt)
	{
		CollisionSpace::SolveManifolds(manifolds, dt); // not sure the order here both ways fuck it up

		for (DynamicsSolver* solver : m_dynamicSolvers) {
			solver->Solve(m_rigidbodies, manifolds, dt);
		}
	}

	void DynamicsSpace::AddRigidbody(
		Rigidbody* rigidbody)
	{
		assert(rigidbody);
		m_rigidbodies.push_back(rigidbody);

		if (rigidbody->TakesGravity()) {
			rigidbody->SetGravity(m_gravity);
		}

		AddCollisionObject(rigidbody);
	}

	void DynamicsSpace::RemoveRigidbody(
		Rigidbody* rigidbody)
	{
		assert(rigidbody);
		auto itr = std::find(m_rigidbodies.begin(), m_rigidbodies.end(), rigidbody);

		assert(itr != m_rigidbodies.end());
		m_rigidbodies.erase(itr);

		RemoveCollisionObject(rigidbody);
	}

	void DynamicsSpace::AddDSolver(
		DynamicsSolver* solver)
	{
		assert(solver);
		m_dynamicSolvers.push_back(solver);
	}

	void DynamicsSpace::RemoveDSolver(
		DynamicsSolver* solver)
	{
		assert(solver);
		auto itr = std::find(m_dynamicSolvers.begin(), m_dynamicSolvers.end(), solver);

		assert(itr != m_dynamicSolvers.end());
		m_dynamicSolvers.erase(itr);
	}

	void DynamicsSpace::Step(
		scalar dt)
	{
		TryApplyGravity();

		PredictTransforms(dt);

		//SweepPredictedBodies();

		//SweepCastBodies(); //CheckCollisions();

		//StepTransforms();
		
		//ResolveTransforms();

		std::vector<Manifold> manifolds;
		for (Rigidbody* a : m_rigidbodies) {
			for (Rigidbody* b : m_rigidbodies) {
				if (a == b) break;

				Manifold m = algo::MakeManifold(a, b);
				if (m.HasCollision) {
					manifolds.push_back(m);
				}
			}
		}

		SolveManifolds(manifolds, dt);

		for (Rigidbody* rigidbody : m_rigidbodies) {
			if (rigidbody->IsKinematic()) {
				if (   isnan(rigidbody->Velocity().length_squared())
					|| isinf(rigidbody->Velocity().length_squared()))
				{
					rigidbody->SetVelocity(0);
				}

				if (   isnan(rigidbody->Trans()->Position.length_squared())
					|| isinf(rigidbody->Trans()->Position.length_squared()))
				{
					rigidbody->Trans()->Position = 0;
				}

				rigidbody->SetVelocity(dt * rigidbody->Force() * rigidbody->Mass() + rigidbody->Velocity());
				rigidbody->Trans()->Position += dt * rigidbody->Velocity();
			}
		}

		// predict where the bodies will be

		// sweep the object from the start to the end and test if there is a collision

		// if there is no collision then the prediction was correct

		// if not then go to the broadphase pair cache and see what other bodies it could be colliding with

		// At the end the forces should be cleared

		// send collision callbacks

		SendCollisionCallbacks(manifolds, dt);

		ClearForces();
	}

	const iw::vector3& DynamicsSpace::Gravity() {
		return m_gravity;
	}

	void DynamicsSpace::SetGravity(
		const iw::vector3& gravity)
	{
		m_gravity = gravity;
		TrySetGravity();
	}

	void DynamicsSpace::TrySetGravity() {
		for (Rigidbody* rigidbody : m_rigidbodies) {
			if (rigidbody->TakesGravity() && rigidbody->IsKinematic()) {
				rigidbody->SetGravity(m_gravity);
			}
		}
	}

	void DynamicsSpace::TryApplyGravity() {
		for (Rigidbody* rigidbody : m_rigidbodies) {
			if (rigidbody->SimGravity() && rigidbody->IsKinematic()) {
				rigidbody->ApplyGravity();
			}
		}
	}

	void DynamicsSpace::PredictTransforms(
		scalar dt)
	{
		for (Rigidbody* rigidbody : m_rigidbodies) {
			if (rigidbody->IsKinematic()) {
				Transform t = *rigidbody->Trans();

				t.Position += 
					  rigidbody->Velocity() * dt
					+ rigidbody->Force() * rigidbody->InvMass() * dt * dt;

				rigidbody->SetNextTrans(t);

				//Transform& t = *rigidbody->Trans();
				//Transform nt = rigidbody->NextTrans();

				//iw::vector3 v = nt.Position - t.Position + dt * rigidbody->Force() * rigidbody->InvMass();

				//nt.Position = t.Position + v * dt;

				////LOG_INFO << v << " " << rigidbody->Velocity();

				//rigidbody->SetNextTrans(t);
				//rigidbody->SetVelocity(v);
			}
		}
	}

	void DynamicsSpace::SweepCastBodies() {
		for (Rigidbody* rigidbody : m_rigidbodies) {
			//ShapeCast(rigidbody, *rigidbody->Trans(), rigidbody->NextTrans());
		}
	}

	void DynamicsSpace::ClearForces() {
		for (Rigidbody* rigidbody : m_rigidbodies) {
			if (rigidbody->IsKinematic()) {
				rigidbody->SetForce(0.0f);
			}
		}
	}
}
