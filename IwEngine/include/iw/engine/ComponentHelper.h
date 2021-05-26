#pragma once

#include "iw/entity/Space.h"
#include "iw/physics/Collision/Manifold.h"

namespace iw {
namespace Engine {
	// Tries to find entities with CollisionObject / Rigidbody components.
	// Returns true if there were no entities.
	// if _t2 is specified, checks if that component exists on entity 2
	template<
		typename _t1,
		typename _t2 = void>
	bool GetEntitiesFromManifold(
		iw::ref<iw::Space>& space,
		const Manifold& manifold,
		iw::Entity& e1,
		iw::Entity& e2)
	{
		iw::Entity a = space->FindEntity(manifold.ObjA);
		iw::Entity b = space->FindEntity(manifold.ObjB);

		if (!a) a = space->FindEntity<iw::Rigidbody>(manifold.ObjA);
		if (!b) b = space->FindEntity<iw::Rigidbody>(manifold.ObjB);

		if (!a || !b) return true; // no physics components

		bool a1 = a.Has<_t1>();
		bool b1 = b.Has<_t1>();

		if (!a1 && !b1) return true; // no t1

		if (b1) { // b has t1
			iw::Entity t = a;
			a = b;
			b = t;
		}

		if constexpr (std::is_same_v<_t2, void> == false) {
			if (!b.Has<_t2>()) { // no t2
				return true;
			}
		}

		e1 = a;
		e2 = b;

		return false;
	}

	inline CollisionObject* GetPhysicsComponent(
		iw::ref<iw::Space>& space,
		iw::EntityHandle e)
	{
		CollisionObject* c = space->FindComponent<CollisionObject>(e);
		if (!c)          c = space->FindComponent<Rigidbody>      (e);

		return c;
	}
	}

	using namespace Engine;
}
