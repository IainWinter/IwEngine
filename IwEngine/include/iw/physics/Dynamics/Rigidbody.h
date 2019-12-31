#pragma once

#include "iw/physics/Collision/CollisionObject.h"
#include "iw/common/Components/Transform.h"

namespace IW {
namespace Physics {
	class Rigidbody
		: public CollisionObject
	{
	private:
		iw::vector3 m_gravity;  // Gravitational force on the rigidbody
		iw::vector3 m_force;    // Net force on the rigidbody
		iw::vector3 m_velocity; // Velocity of rigidbody
		scalar m_invMass;       // 1 / Mass of rigidbody
		bool m_takesGravity;    // If the rigidbody will use its own gravity or take it from the space
		bool m_simGravity;      // If the rigidbody will simulate gravity
		bool m_isKinematic;     // If the rigidbody gets simulated. Still participates with collisions, but is unefected.

		float m_staticFriction;   // Static friction coeffeicent
		float m_dynamicFriction;  // Dynamic friction coeffeicent
		float m_restitution;      // Elasticity of collisions

		Transform   m_nextTrans;     // Where the rigidbody will be next step if there is no interference
		iw::vector3 m_interVelocity; // Velocity from positional change

	public:
		IWPHYSICS_API
		Rigidbody(
			bool isKinematic = true);

		IWPHYSICS_API
		void ApplyForce(
			const iw::vector3& force);

		IWPHYSICS_API
		void ApplyGravity();

		// Transforms the rigidbody by its center of mass (no torque)
		IWPHYSICS_API
		void TransCenterOfMass(
			const Transform& to);

		IWPHYSICS_API
		const iw::vector3& Gravity() const;

		IWPHYSICS_API
		const iw::vector3& Force() const;

		IWPHYSICS_API
		const iw::vector3& Velocity() const;

		IWPHYSICS_API
		scalar Mass() const;

		IWPHYSICS_API
		scalar InvMass() const;

		IWPHYSICS_API
		bool TakesGravity() const;

		IWPHYSICS_API
		bool SimGravity() const;

		IWPHYSICS_API
		bool IsKinematic() const;

		IWPHYSICS_API
		float StaticFriction() const;

		IWPHYSICS_API
		float DynamicFriction() const;

		IWPHYSICS_API
		float Restitution() const;

		IWPHYSICS_API
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
		void SetIsKinematic(
			bool isKinematic);

		IWPHYSICS_API
		void SetStaticFriction(
			float staticFriction);

		IWPHYSICS_API
		void SetDynamicFriction(
			float dynamicFriction);

		IWPHYSICS_API
		void SetRestitution(
			float restitution);

		IWPHYSICS_API
		void SetNextTrans(
			const Transform& nextTrans);
	};
}

	using namespace Physics;
}
