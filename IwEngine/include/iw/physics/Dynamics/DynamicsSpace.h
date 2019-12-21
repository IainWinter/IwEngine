#pragma once

#include "iw/physics/Collision/CollisionSpace.h"
#include "RigidbodySolver.h"

namespace IW {
namespace Physics {
	class DynamicsSpace
		: public CollisionSpace
	{
	private:
		std::vector<Rigidbody*> m_rigidbodies;
		std::vector<RigidbodySolver*> m_rigidbodySolvers;
		iw::vector3 m_gravity;
		
	public:
		IWPHYSICS_API
		virtual void AddRigidbody(
			Rigidbody* rigidbody);

		IWPHYSICS_API
		virtual void RemoveRigidbody(
			Rigidbody* rigidbody);

		IWPHYSICS_API
		void AddSolver(
			RigidbodySolver* solver);

		IWPHYSICS_API
		void RemoveSolver(
			RigidbodySolver* solver);

		IWPHYSICS_API
		virtual void Step(
			scalar dt);

		const iw::vector3& Gravity();

		IWPHYSICS_API
		void SetGravity(
			const iw::vector3& gravity);
	protected:
		
	private:
		void TrySetGravity();
		void TryApplyGravity();

		void PredictTransforms(
			scalar dt);

		void SweepCastBodies();

		void ClearForces();
	};
}

	using namespace Physics;
}
