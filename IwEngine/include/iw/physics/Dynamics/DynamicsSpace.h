#pragma once

#include "iw/physics/Collision/CollisionSpace.h"
#include "DynamicsSolver.h"

namespace iw {
namespace Physics {
	class DynamicsSpace
		: public CollisionSpace
	{
	private:
		std::vector<Rigidbody*> m_rigidbodies;
		std::vector<DynamicsSolver*> m_dynamicSolvers;
		iw::vector3 m_gravity;

	public:
		IWPHYSICS_API
		virtual void AddRigidbody(
			Rigidbody* rigidbody);

		IWPHYSICS_API
		virtual void RemoveRigidbody(
			Rigidbody* rigidbody);

		IWPHYSICS_API
		void AddDSolver(
			DynamicsSolver* solver);

		IWPHYSICS_API
		void RemoveDSolver(
			DynamicsSolver* solver);

		IWPHYSICS_API
		virtual void Step(
			scalar dt);

		const iw::vector3& Gravity();

		IWPHYSICS_API
		void SetGravity(
			const iw::vector3& gravity);
	protected:
		IWPHYSICS_API
		void SolveManifolds(
			std::vector<Manifold>& manifolds,
			scalar dt = 0) override;
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
