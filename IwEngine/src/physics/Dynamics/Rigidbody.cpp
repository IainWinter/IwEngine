#include "iw/physics/Dynamics/Rigidbody.h"

namespace iw {
namespace Physics {
	Rigidbody::Rigidbody(
		bool isKinematic)
		: CollisionObject()
		, InvMass(1)
		, TakesGravity(true)
		, SimGravity(true)
		, IsKinematic(isKinematic)
		, StaticFriction(.5)
		, DynamicFriction(.5)
		, Restitution(.5)
	{
		m_isDynamic = true;
	}

	void Rigidbody::ApplyForce(
		const iw::vector3& force)
	{
		NetForce += force;
	}

	void Rigidbody::ApplyGravity() {
		ApplyForce(Gravity * Mass());
	}

	scalar Rigidbody::Mass() const {
		return 1.0f / InvMass;
	}

	const Transform& Rigidbody::LastTrans() const {
		return m_lastTrans;
	}

	const Transform& Rigidbody::NextTrans() const {
		return m_nextTrans;
	}

	void Rigidbody::SetMass(
		scalar mass)
	{
		InvMass = 1.0f / mass;
	}

	void Rigidbody::SetTrans(
		Transform* transform)
	{
		m_nextTrans = *transform;
		m_lastTrans = *transform;
		CollisionObject::SetTrans(transform);
	}

	void Rigidbody::SetLastTrans(
		const Transform& lastTrans)
	{
		m_lastTrans = lastTrans;
	}
	
	void Rigidbody::SetNextTrans(
		const Transform& nextTrans)
	{
		m_nextTrans = nextTrans;
	}

	void Rigidbody::Move(
		const iw::vector3 delta)
	{		
		m_transform.Position += delta;
		Velocity = delta;
	}
}
}
