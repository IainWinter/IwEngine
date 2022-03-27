#include "iw/physics/Dynamics/Rigidbody.h"

namespace iw {
namespace Physics {
	Rigidbody::Rigidbody(
		bool isKinematic
	)
		: CollisionObject(true)
		, Gravity(0.f)
		, NetForce(0.f)
		, Velocity(0.f)
		, NetTorque(0.f)
		, AngularVelocity(0.f)
		, Inertia(1.f)
		, AxisLock(0.f)
		, IsAxisLocked(0.f)
		, InvMass(1)
		, TakesGravity(true)
		, SimGravity(true)
		, IsSimulated(isKinematic)
		, StaticFriction(.5)
		, DynamicFriction(.5)
		, Restitution(.5)
	{}

	//void Rigidbody::ApplyForce2(
	//	const glm::vec2& force, 
	//	const glm::vec2& position)
	//{
	//	NetForce += glm::vec3(force, 0);
	//	ApplyTorque2();
	//}

	//void Rigidbody::ApplyTorque2(
	//	float torque)
	//{
	//	NetTorque.z += torque;
	//}

	void Rigidbody::ApplyForce(
		const glm::vec3& force,
		const glm::vec3& position)
	{
		NetForce += force;
		ApplyTorque(glm::cross(position, force));
	}

	void Rigidbody::ApplyTorque(
		const glm::vec3& torque)
	{
		NetTorque += torque;
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

	void Rigidbody::SetTransform(
		iw::Transform* transform)
	{
		SetLastTrans(*transform);
		SetNextTrans(*transform);
		CollisionObject::SetTransform(transform);
	}

	void Rigidbody::SetLastTrans(
		const iw::Transform& lastTrans)
	{
		m_lastTrans = lastTrans;
	}
	
	void Rigidbody::SetNextTrans(
		const iw::Transform& nextTrans)
	{
		m_nextTrans = nextTrans;
	}

	void Rigidbody::Move(
		const glm::vec3 delta)
	{		
		Transform.Position += delta;
		Velocity = delta;
	}
}
}
