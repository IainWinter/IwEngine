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
		std::vector<DynamicSolver*> m_dynamicSolvers;
		iw::vector3 m_gravity;

	protected:
		IWPHYSICS_API
		void SolveManifolds(
			std::vector<Manifold>& manifolds,
			scalar dt = 0) override;
	public:
		IWPHYSICS_API
		virtual void AddRigidbody(
			Rigidbody* rigidbody);

		IWPHYSICS_API
		virtual void RemoveRigidbody(
			Rigidbody* rigidbody);

		IWPHYSICS_API
		void AddDSolver(
			DynamicSolver* solver);

		IWPHYSICS_API
		void RemoveDSolver(
			DynamicSolver* solver);

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
