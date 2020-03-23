#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Time.h"

namespace iw {
namespace Engine {
	PhysicsSystem::PhysicsSystem()
		: System<Transform, Rigidbody>("Physics")
	{}

	void PhysicsSystem::Update(
		EntityComponentArray& eca)
	{
		for (auto entity : eca) {
			auto [transform, rigidbody] = entity.Components.Tie<Components>();
			
			if (rigidbody->IsKinematic()) {
				transform->Position = iw::lerp(rigidbody->LastTrans().Position, rigidbody->Trans()->Position, accumulator / iw::FixedTime());
			}
		}

		accumulator += Time::DeltaTime();
	}

	void PhysicsSystem::FixedUpdate(
		EntityComponentArray& view)
	{
		accumulator = 0;
	}

	bool PhysicsSystem::On(
		EntityDestroyedEvent& e)
	{
		CollisionObject* object = e.Entity.FindComponent<CollisionObject>();
		if (!object) {
			object = e.Entity.FindComponent<Rigidbody>();
		}

		if (object) {
			Physics->RemoveCollisionObject(object);
		}

		return false;
	}
}
}
