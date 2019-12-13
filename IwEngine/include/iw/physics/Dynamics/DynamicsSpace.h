#pragma once

#include "iw/physics/IwPhysics.h"
#include "Rigidbody.h"
#include "iw/physics/Collision/CollisionSpace.h"
#include "iw/common/Components/Transform.h"
#include <vector>

namespace IW {
namespace Physics {
	class DynamicsSpace
		: public CollisionSpace
	{
	private:
		std::vector<Rigidbody*> m_rigidbodies;
		iw::vector3 m_gravity;

	public:
		IWPHYSICS_API
		virtual void AddRigidbody(
			Rigidbody* rigidbody);

		IWPHYSICS_API
		virtual void RemoveRigidbody(
			Rigidbody* rigidbody);

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
