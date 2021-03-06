#pragma once

#include "iw/physics/Collision/CollisionObject.h"
#include "iw/common/Components/Transform.h"

namespace iw {
namespace Physics {
	class Rigidbody
		: public CollisionObject
	{
	private:
		vector3 m_gravity;  // Gravitational acceleration 
		vector3 m_force;    // Net force
		vector3 m_velocity;
		vector3 m_lock;     // Lock to axis
		vector3 m_isLocked; // If its locked to each axis casted to bool
		scalar m_invMass;       // 1 / Mass of rigidbody
		bool m_takesGravity;    // If the rigidbody will use its own gravity or take it from the space
		bool m_simGravity;      // If the rigidbody will simulate gravity
		bool m_isKinematic;     // If the rigidbody gets simulated. Still participates with collisions, but is unefected.

		scalar m_staticFriction;  // Static friction coefficient
		scalar m_dynamicFriction; // Dynamic friction coefficient
		scalar m_restitution;     // Elasticity of collisions

		Transform m_lastTrans; // Where the rigidbody was last step
		Transform m_nextTrans; // Where the rigidbody will be next step if there is no interference

	public:
		IWPHYSICS_API
		Rigidbody(
			bool isKinematic = true);

		IWPHYSICS_API
		void ApplyForce(
			const iw::vector3& force);

		IWPHYSICS_API
		void ApplyGravity();

		IWPHYSICS_API
		const iw::vector3& Gravity() const;

		IWPHYSICS_API
		const iw::vector3& Force() const;

		IWPHYSICS_API
		const iw::vector3& Velocity() const;

		IWPHYSICS_API
		const iw::vector3& Lock() const;

		IWPHYSICS_API
		const iw::vector3& IsLocked() const;

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
		const Transform& LastTrans() const;

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
		void SetLock(
			const iw::vector3& lock);

		IWPHYSICS_API
		void SetIsLocked(
			const iw::vector3& isLocked);

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
			scalar staticFriction);

		IWPHYSICS_API
		void SetDynamicFriction(
			scalar dynamicFriction);

		IWPHYSICS_API
		void SetRestitution(
			scalar restitution);

		IWPHYSICS_API
		void SetTrans(
			Transform* transform) override;

		IWPHYSICS_API
		void SetLastTrans(
			const Transform& lastTrans);

		IWPHYSICS_API
		void SetNextTrans(
			const Transform& nextTrans);

		IWPHYSICS_API
		void Move(
			const iw::vector3 delta);
	};
}

	using namespace Physics;
}
