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

		if (rigidbody->TakesGravity()) {
			rigidbody->SetGravity(m_gravity);
		}

		AddCollisionObject(rigidbody);
	}

	void DynamicsSpace::Step(
		scalar dt)
	{
		TryApplyGravity();

		PredictTransforms(dt);

		//SweepPredictedBodies();

		ResolveConstrains(dt); // might go after step

		for (CollisionObject* object : m_objects) {
			if (!object->IsDynamic()) continue;

			Rigidbody* rigidbody = (Rigidbody*)object;

			rigidbody->SetLastTrans(rigidbody->Trans());

			if (rigidbody->IsKinematic()) {
				if (   isnan(rigidbody->Velocity().length_squared())
					|| isinf(rigidbody->Velocity().length_squared()))
				{
					rigidbody->SetVelocity(0);
				}

				if (   isnan(rigidbody->Trans().Position.length_squared())
					|| isinf(rigidbody->Trans().Position.length_squared()))
				{
					rigidbody->Trans().Position = 0;
				}

				rigidbody->SetVelocity(dt * rigidbody->Force() * rigidbody->InvMass() + rigidbody->Velocity());
				rigidbody->Trans().Position += dt * rigidbody->Velocity();

				if (rigidbody->IsLocked().x) {
					rigidbody->Trans().Position.x = rigidbody->Lock().x;
				}

				if (rigidbody->IsLocked().y) {
					rigidbody->Trans().Position.y = rigidbody->Lock().y;
				}

				if (rigidbody->IsLocked().z) {
					rigidbody->Trans().Position.z = rigidbody->Lock().z;
				}
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
		for (CollisionObject* object : m_objects) {
			if (!object->IsDynamic()) continue;

			Rigidbody* rigidbody = (Rigidbody*)object;
			if (rigidbody->TakesGravity() && rigidbody->IsKinematic()) {
				rigidbody->SetGravity(m_gravity);
			}
		}
	}

	void DynamicsSpace::TryApplyGravity() {
		for (CollisionObject* object : m_objects) {
			if (!object->IsDynamic()) continue;

			Rigidbody* rigidbody = (Rigidbody*)object;
			if (rigidbody->SimGravity() && rigidbody->IsKinematic()) {
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
			if (rigidbody->IsKinematic()) {
				Transform t = rigidbody->Trans();
				vector3   v = rigidbody->Velocity();

				if (rigidbody->IsLocked().x) {
					t.Position.x = rigidbody->Lock().x;
					v.x = 0;
				}

				if (rigidbody->IsLocked().y) {
					t.Position.y = rigidbody->Lock().y;
					v.y = 0;
				}

				if (rigidbody->IsLocked().z) {
					t.Position.z = rigidbody->Lock().z;
					v.z = 0;
				}

				t.Position += dt * v + dt * dt * rigidbody->Force() * rigidbody->InvMass();

				rigidbody->SetVelocity(v);
				rigidbody->SetNextTrans(t);
			}
		}
	}

	void DynamicsSpace::ClearForces() {
		for (CollisionObject* object : m_objects) {
			if (!object->IsDynamic()) continue;
			Rigidbody* rigidbody = (Rigidbody*)object;

			if (rigidbody->IsKinematic()) {
				rigidbody->SetForce(0.0f);
			}
		}
	}
}
}
