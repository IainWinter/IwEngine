#pragma once

#include "iw/common/Components/Transform.h"
#include "iw/physics/Collision/Collider.h"
#include "iw/events/callback.h"

namespace iw {
namespace Physics {
	struct Manifold;

	using func_CollisionCallback = std::function<void(Manifold&, scalar)>;

	class CollisionObject {
	protected:
		Transform m_transform;     // Position, rotation, and scale.
		Collider* m_collider;      // Shape of the collision object (only 3d right now)
		bool m_isTrigger;
		bool m_isStatic;
		bool m_isDynamic;

		func_CollisionCallback m_onCollision;

	public:
		IWPHYSICS_API
		CollisionObject();

		IWPHYSICS_API
		AABB Bounds() const;

		IWPHYSICS_API
		Transform& Trans();

		IWPHYSICS_API
		const Transform& Trans() const;

		IWPHYSICS_API
		Transform ColTrans() const; // not sure about this

		IWPHYSICS_API
		Collider* Col() const; // should be const probly also these are not needed if they are pointers but Ill try the get set thigs for now as a lil test because ive always dismissed it for the most part

		IWPHYSICS_API
		bool IsTrigger() const;

		IWPHYSICS_API
		bool IsStatic() const;

		IWPHYSICS_API
		bool IsDynamic() const; // if the collision object is a rigidbody

		IWPHYSICS_API
		func_CollisionCallback& OnCollision();

		IWPHYSICS_API
		virtual void SetTrans(
			Transform* transform);

		IWPHYSICS_API
		void SetCol(
			Collider* collider);

		IWPHYSICS_API
		void SetIsTrigger(
			bool isTrigger);

		IWPHYSICS_API
		void SetIsStatic(
			bool isStatic);

		IWPHYSICS_API
		void SetOnCollision(
			iw::func_CollisionCallback callback);
	};
}

	using namespace Physics;
}
