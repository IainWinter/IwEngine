#pragma once

#include "iw/physics/Collision/CollisionObject.h"
#include "iw/common/Components/Transform.h"

namespace IW {
namespace Physics {
	class Rigidbody
		: public CollisionObject
	{
	private:
		iw::vector3 m_gravity;     // Gravitational force on the Rigidbody
		iw::vector3 m_force;       // Net force on the Rigidbody
		bool m_takesGravity;       // If the Rigidbody will use its own gravity or take it from the space
		bool m_simGravity;         // If the Rigidbody will simulate gravity

	public:
		IWPHYSICS_API
		void ApplyForce(
			const iw::vector3& force);

		IWPHYSICS_API
		void ApplyGravity();

		const iw::vector3& Gravity() const;
		const iw::vector3& Force() const;
		bool TakesGravity() const;
		bool SimGravity() const;

		IWPHYSICS_API
		void SetGravity(
			const iw::vector3& gravity);

		IWPHYSICS_API
		void SetForce(
			const iw::vector3& force);

		IWPHYSICS_API
		void SetTakesGravity(
			bool takesGravity);

		IWPHYSICS_API
		void SetSimGravity(
			bool simGravity);
	};
}

	using namespace Physics;
}
