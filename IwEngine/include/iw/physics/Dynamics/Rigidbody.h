#pragma once

#include "iw/physics/Collision/CollisionObject.h"
#include "iw/common/Components/Transform.h"

namespace IW {
namespace Physics {
	class Rigidbody
		: public CollisionObject
	{
	private:
		iw::vector3 m_gravity;     // Gravitational force on the rigidbody
		iw::vector3 m_force;       // Net force on the rigidbody
		iw::vector3 m_velocity;    // Velocity of rigidbody
		scalar m_invMass;          // 1 / Mass of rigidbody
		bool m_takesGravity;       // If the rigidbody will use its own gravity or take it from the space
		bool m_simGravity;         // If the rigidbody will simulate gravity

		Transform  m_nextTrans;    // Where the rigidbody will be next step if there is no interference

	public:
		IWPHYSICS_API
		void ApplyForce(
			const iw::vector3& force);

		IWPHYSICS_API
		void ApplyGravity();

		// Transforms the rigidbody by its center of mass (no torque)
		IWPHYSICS_API
		void TransCenterOfMass(
			const Transform& to);

		const iw::vector3& Gravity() const;
		const iw::vector3& Force() const;
		const iw::vector3& Velocity() const;
		scalar Mass() const;
		scalar InvMass() const;
		bool TakesGravity() const;
		bool SimGravity() const;
		const Transform& NextTrans() const;

		IWPHYSICS_API
		void SetGravity(
			const iw::vector3& gravity);

		IWPHYSICS_API
		void SetForce(
			const iw::vector3& force);

		IWPHYSICS_API
		void SetVelocity(
			const iw::vector3& velocity);

		IWPHYSICS_API
		void SetMass(
			scalar mass);

		IWPHYSICS_API
		void SetTakesGravity(
			bool takesGravity);

		IWPHYSICS_API
		void SetSimGravity(
			bool simGravity);

		IWPHYSICS_API
		void SetNextTrans(
			const Transform& nextTrans);
	};
}

	using namespace Physics;
}
