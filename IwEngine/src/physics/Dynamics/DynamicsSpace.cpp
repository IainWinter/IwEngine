#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/physics/Collision/algo/ManifoldFactory.h"
#include "iw/log/logger.h"
#include <assert.h>
#include <vector>

namespace iw {
namespace Physics {
 	void DynamicsSpace::AddRigidbody(
		Rigidbody* rigidbody)
	{
#ifdef IW_DEBUG
		if (rigidbody == nullptr) {
			LOG_WARNING << "Tried to add null rigidbody to dynamics space";
			return;
		}
#endif

		if (rigidbody->TakesGravity) {
			rigidbody->Gravity = m_gravity;
		}

		AddCollisionObject(rigidbody);
	}

	void DynamicsSpace::Step(
		scalar dt)
	{
		TryApplyGravity();

		//PredictTransforms(dt);

		//SweepPredictedBodies();

		ResolveConstrains(dt); // might go after step

		for (CollisionObject* object : m_objects) {
			if (!object->IsDynamic()) continue;

			Rigidbody* rigidbody = (Rigidbody*)object;

			rigidbody->SetLastTrans(rigidbody->Trans());

			if (rigidbody->IsKinematic) {

				Transform& transform = rigidbody->Trans();

				if (   isnan(glm::length2(rigidbody->Velocity))
					|| isinf(glm::length2(rigidbody->Velocity)))
				{
					rigidbody->Velocity = glm::vec3();
				}

				if (   isnan(glm::length2(rigidbody->Trans().Position))
					|| isinf(glm::length2(rigidbody->Trans().Position)))
				{
					transform.Position = glm::vec3();
				}

				rigidbody->Velocity += dt * rigidbody->NetForce * rigidbody->InvMass;
				transform. Position += dt * rigidbody->Velocity;

				rigidbody->AngularVelocity += dt * rigidbody->NetTorque * rigidbody->Inertia;

				glm::quat rot = glm::angleAxis(
					glm::length(rigidbody->AngularVelocity) * dt, 
					glm::length(rigidbody->AngularVelocity) == 0 ? glm::vec3(0, 0, 1) 
																 : glm::normalize(rigidbody->AngularVelocity)
				);

				transform.Rotation = rot * transform.Rotation;
				
				// Axis lock should be through constraints I think

				if (rigidbody->IsAxisLocked.x) {
					rigidbody->Trans().Position.x = rigidbody->AxisLock.x;
				}

				if (rigidbody->IsAxisLocked.y) {
					rigidbody->Trans().Position.y = rigidbody->AxisLock.y;
				}

				if (rigidbody->IsAxisLocked.z) {
					rigidbody->Trans().Position.z = rigidbody->AxisLock.z;
				}
			}
		}

		ClearForces();
	}

	const glm::vec3& DynamicsSpace::Gravity() {
		return m_gravity;
	}

	void DynamicsSpace::SetGravity(
		const glm::vec3& gravity)
	{
		m_gravity = gravity;
		TrySetGravity();
	}

	void DynamicsSpace::TrySetGravity() {
		for (CollisionObject* object : m_objects) {
			if (!object->IsDynamic()) continue;

			Rigidbody* rigidbody = (Rigidbody*)object;
			if (rigidbody->TakesGravity && rigidbody->IsKinematic) {
				rigidbody->Gravity = m_gravity;
			}
		}
	}

	void DynamicsSpace::TryApplyGravity() {
		for (CollisionObject* object : m_objects) {
			if (!object->IsDynamic()) continue;

			Rigidbody* rigidbody = (Rigidbody*)object;
			if (rigidbody->SimGravity && rigidbody->IsKinematic) {
				rigidbody->ApplyGravity();
			}
		}
	}

	void DynamicsSpace::PredictTransforms(
		scalar dt)
	{
		for (CollisionObject* object : m_objects) {
			if (!object->IsDynamic()) continue;

			Rigidbody* rigidbody = (Rigidbody*)object;
			if (rigidbody->IsKinematic) {
				Transform t = rigidbody->Trans();
				glm::vec3   v = rigidbody->Velocity;

				if (rigidbody->IsAxisLocked.x) {
					t.Position.x = rigidbody->AxisLock.x;
					v.x = 0;
				}

				if (rigidbody->IsAxisLocked.y) {
					t.Position.y = rigidbody->AxisLock.y;
					v.y = 0;
				}

				if (rigidbody->IsAxisLocked.z) {
					t.Position.z = rigidbody->AxisLock.z;
					v.z = 0;
				}

				t.Position += dt * v + dt * dt * rigidbody->NetForce * rigidbody->InvMass;

				rigidbody->Velocity = v;
				rigidbody->SetNextTrans(t);
			}
		}
	}

	void DynamicsSpace::ClearForces() {
		for (CollisionObject* object : m_objects) {
			if (!object->IsDynamic()) continue;
			Rigidbody* rigidbody = (Rigidbody*)object;

			if (rigidbody->IsKinematic) {
				rigidbody->NetForce  = glm::vec3();
				rigidbody->NetTorque = glm::vec3();
			}
		}
	}
}
}
