#pragma once

#include "iw/physics/Collision/CollisionSpace.h"
//#include "Rigidbody.h"
#include "Mechanism.h"
//#include "iw/physics/Collision/TimedSolver.h"

namespace iw {
namespace Physics {
	class DynamicsSpace
		: public CollisionSpace
	{
	public:
		bool Paused = false;

	private:
		//std::vector<Rigidbody*> m_rigidbodies;
		glm::vec3 m_gravity;

		//std::vector<Mechanism*> m_mechanisms;
		std::vector<Constraint*> m_constraints;

	public:
		IWPHYSICS_API
		virtual void AddRigidbody(
			Rigidbody* rigidbody);

		/*IWPHYSICS_API
		virtual void AddMechanism(
			Mechanism* mechanism);*/

		IWPHYSICS_API
		virtual void AddConstraint(
			Constraint* constraint);

		IWPHYSICS_API
		virtual void Step(
			scalar dt);

		IWPHYSICS_API
		const glm::vec3& Gravity();

		IWPHYSICS_API
		void SetGravity(
			const glm::vec3& gravity);

		IWPHYSICS_API
		const std::vector<Constraint*>& Constraints() const;
	private:
		void TrySetGravity();
		void TryApplyGravity();

		void PredictTransforms(
			scalar dt);

		void ClearForces();
	};
}

	using namespace Physics;
}
