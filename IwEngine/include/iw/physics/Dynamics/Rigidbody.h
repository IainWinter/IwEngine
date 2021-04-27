#pragma once

#include "iw/physics/Collision/CollisionObject.h"
#include "iw/common/Components/Transform.h"

namespace iw {
namespace Physics {
	class Rigidbody
		: public CollisionObject
	{
	public:
		glm::vec3 Gravity;  // Gravitational acceleration 
		glm::vec3 NetForce;    // Net force
		glm::vec3 Velocity;

		glm::vec3 NetTorque;
		glm::vec3 AngularVelocity;
		glm::mat3 Inertia = glm::mat3(1); // for testing

		glm::vec3 AxisLock;     // Lock to axis
		glm::vec3 IsAxisLocked; // If its locked to each axis casted to bool

		scalar InvMass;       // 1 / Mass of rigidbody
		bool TakesGravity;    // If the rigidbody will use its own gravity or take it from the space
		bool SimGravity;      // If the rigidbody will simulate gravity
		bool IsKinematic;     // If the rigidbody gets simulated. Still participates with collisions, but is unefected.

		scalar StaticFriction;  // Static friction coefficient
		scalar DynamicFriction; // Dynamic friction coefficient
		scalar Restitution;     // Elasticity of collisions
	private:
		Transform m_lastTrans; // Where the rigidbody was last step
		Transform m_nextTrans; // Where the rigidbody will be next step if there is no interference

	public:
		IWPHYSICS_API Rigidbody(bool isKinematic = true);

		IWPHYSICS_API void ApplyForce (const glm::vec3& force, const glm::vec3& position = glm::vec3());
		IWPHYSICS_API void ApplyTorque(const glm::vec3& torque);
		IWPHYSICS_API void ApplyGravity();

		IWPHYSICS_API const Transform& LastTrans() const;
		IWPHYSICS_API const Transform& NextTrans() const;

		IWPHYSICS_API scalar  Mass() const;
		IWPHYSICS_API void SetMass(scalar mass);

		IWPHYSICS_API void SetTrans(Transform* transform) override;
		IWPHYSICS_API void SetLastTrans(const Transform& lastTrans);
		IWPHYSICS_API void SetNextTrans(const Transform& nextTrans);

		IWPHYSICS_API void Move(const glm::vec3 delta);
	};
}

	using namespace Physics;
}
