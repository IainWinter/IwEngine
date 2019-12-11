#include "iw/physics/Dynamics/Rigidbody.h"

namespace IW {
	void Rigidbody::ApplyForce(
		const iw::vector3& force)
	{
		m_force += force;
	}

	void Rigidbody::ApplyGravity() {
		ApplyForce(m_gravity);
	}

	const iw::vector3& Rigidbody::Gravity() const {
		return m_gravity;
	}

	const iw::vector3& Rigidbody::Force() const {
		return m_force;
	}

	bool Rigidbody::TakesGravity() const {
		return m_takesGravity;
	}

	bool Rigidbody::SimGravity() const {
		return m_simGravity;
	}

	void Rigidbody::SetGravity(
		const iw::vector3& gravity)
	{
		m_gravity = gravity;
	}

	void Rigidbody::SetForce(
		const iw::vector3& force)
	{
		m_force = force;
	}

	void Rigidbody::SetTakesGravity(
		bool takesGravity)
	{
		m_takesGravity = takesGravity;
	}

	void Rigidbody::SetSimGravity(
		bool simGravity)
	{
		m_simGravity = simGravity;
	}
}
