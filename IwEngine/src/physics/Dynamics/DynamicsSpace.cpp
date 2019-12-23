#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"
#include "iw/log/logger.h"
#include <assert.h>
#include <vector>

namespace IW {
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

	void DynamicsSpace::AddSolver(
		RigidbodySolver* solver)
	{
		assert(solver);
		m_rigidbodySolvers.push_back(solver);
	}

	void DynamicsSpace::RemoveSolver(
		RigidbodySolver* solver)
	{
		assert(solver);
		auto itr = std::find(m_rigidbodySolvers.begin(), m_rigidbodySolvers.end(), solver);

		assert(itr != m_rigidbodySolvers.end());
		m_rigidbodySolvers.erase(itr);
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

		for (RigidbodySolver* solver : m_rigidbodySolvers) {
			solver->Solve(m_rigidbodies, dt);
		}

		for (Rigidbody* rigidbody : m_rigidbodies) {
			rigidbody->SetVelocity(dt * rigidbody->Force() / rigidbody->Mass() + rigidbody->Velocity());
			rigidbody->Trans()->Position += rigidbody->Velocity();
		}

		// predict where the bodies will be

		// sweep the object from the start to the end and test if there is a collision

		// if there is no collision then the prediction was correct

		// if not then go to the broadphase pair cache and see what other bodies it could be colliding with

		// At the end the forces should be cleared
 
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
			if (rigidbody->TakesGravity()) {
				rigidbody->SetGravity(m_gravity);
			}
		}
	}

	void DynamicsSpace::TryApplyGravity() {
		for (Rigidbody* rigidbody : m_rigidbodies) {
			if (rigidbody->SimGravity()) {
				rigidbody->ApplyGravity();
			}
		}
	}

	void DynamicsSpace::PredictTransforms(
		scalar dt)
	{
		for (Rigidbody* rigidbody : m_rigidbodies) {
			Transform t = *rigidbody->Trans();

			t.Position += rigidbody->Velocity() + dt * rigidbody->Force() / rigidbody->Mass();
			rigidbody->SetNextTrans(t);
		}
	}

	void DynamicsSpace::SweepCastBodies() {
		for (Rigidbody* rigidbody : m_rigidbodies) {
			//ShapeCast(rigidbody, *rigidbody->Trans(), rigidbody->NextTrans());
		}
	}

	void DynamicsSpace::ClearForces() {
		for (Rigidbody* rigidbody : m_rigidbodies) {
			rigidbody->SetForce(0.0f);
		}
	}
}
