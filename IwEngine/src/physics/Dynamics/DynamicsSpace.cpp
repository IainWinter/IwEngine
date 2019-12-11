#include "iw/physics/Dynamics/DynamicsSpace.h"
#include <assert.h>

namespace IW {
	void DynamicsSpace::AddRigidbody(
		Rigidbody* rigidbody)
	{
		assert(rigidbody);
		m_rigidbodies.push_back(rigidbody);

		if (rigidbody->TakesGravity()) {
			rigidbody->SetGravity(m_gravity);
		}
	}

	void DynamicsSpace::RemoveRigidbody(
		Rigidbody* rigidbody)
	{
		assert(rigidbody);
		auto itr = std::find(m_rigidbodies.begin(), m_rigidbodies.end(), rigidbody);

		assert(itr != m_rigidbodies.end());
		m_rigidbodies.erase(itr);
	}

	void DynamicsSpace::Step(
		float dt)
	{
		// predict where the bodies will be

		// sweep the object from the start to the end and test if there is a collision

		// if there is no collision then the prediction was correct

		// if not then go to the broadphase pair cache and see what other bodies it could be colliding with

			// objects should take the average of all these forces? I think that you could just keep adding them together??


		// At the end the forces should be cleared


		for (Rigidbody* rigidbody : m_rigidbodies) {
			if (rigidbody->SimGravity()) {
				rigidbody->ApplyGravity();
			}
		}

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

	void DynamicsSpace::ClearForces() {
		for (Rigidbody* rigidbody : m_rigidbodies) {
			rigidbody->SetForce(0.0f);
		}
	}
}
