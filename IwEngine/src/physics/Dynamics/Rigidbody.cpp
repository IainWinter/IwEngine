#include "iw/physics/Dynamics/Rigidbody.h"

namespace IW {
	Rigidbody::Rigidbody(
		bool isKinematic)
		: m_invMass(1)
		, m_takesGravity(true)
		, m_simGravity(true)
		, m_isKinematic(isKinematic)
		, m_staticFriction(.5)
		, m_dynamicFriction(.5)
		, m_restitution(.5)
	{}

	void Rigidbody::ApplyForce(
		const iw::vector3& force)
	{
		m_force += force;
	}

	void Rigidbody::ApplyGravity() {
		ApplyForce(m_gravity * Mass());
	}

	const iw::vector3& Rigidbody::Gravity() const {
		return m_gravity;
	}

	const iw::vector3& Rigidbody::Force() const {
		return m_force;
	}

	const iw::vector3& Rigidbody::Velocity() const {
		return m_velocity;
	}

	scalar Rigidbody::Mass() const {
		return 1.0f / m_invMass;
	}

	scalar Rigidbody::InvMass() const {
		return m_invMass;
	}

	bool Rigidbody::TakesGravity() const {
		return m_takesGravity;
	}

	bool Rigidbody::SimGravity() const {
		return m_simGravity;
	}

	bool Rigidbody::IsKinematic() const {
		return m_isKinematic;
	}

	float Rigidbody::StaticFriction() const {
		return m_staticFriction;
	}

	float Rigidbody::DynamicFriction() const {
		return m_dynamicFriction;
	}

	float Rigidbody::Restitution() const {
		return m_restitution;
	}

	const Transform& Rigidbody::NextTrans() const {
		return m_nextTrans;
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

	void Rigidbody::SetVelocity(
		const iw::vector3& velocity)
	{
		m_velocity = velocity;
	}

	void Rigidbody::SetMass(
		scalar mass)
	{
		m_invMass = 1.0f / mass;
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

	void Rigidbody::SetIsKinematic(
		bool isKinematic)
	{
		m_isKinematic = isKinematic;
	}

	void Rigidbody::SetStaticFriction(
		float staticFriction)
	{
		m_staticFriction = staticFriction;
	}

	void Rigidbody::SetDynamicFriction(
		float dynamicFriction)
	{
		m_dynamicFriction = dynamicFriction;
	}

	void Rigidbody::SetRestitution(
		float restitution)
	{
		m_restitution = restitution;
	}
	
	void Rigidbody::SetNextTrans(
		const Transform& nextTrans)
	{
		m_nextTrans = nextTrans;
	}
}
