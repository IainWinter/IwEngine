#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Time.h"

namespace iw {
namespace Engine {
	PhysicsSystem::PhysicsSystem()
		: System<Transform, Rigidbody>("Physics")
	{}

	struct OtherComponents {
		Transform* Transform;
		CollisionObject* Object;
	};

	void PhysicsSystem::Update(
		EntityComponentArray& eca)
	{
		for (auto entity : eca) {
			auto [transform, rigidbody] = entity.Components.Tie<Components>();
			
			if (rigidbody->IsKinematic()) {
				transform->Position = iw::lerp(rigidbody->LastTrans().Position, rigidbody->Trans().Position, accumulator / iw::FixedTime());
				transform->Scale    = iw::lerp(rigidbody->LastTrans().Scale,    rigidbody->Trans().Scale,    accumulator / iw::FixedTime());
				transform->Rotation = iw::lerp(rigidbody->LastTrans().Rotation, rigidbody->Trans().Rotation, accumulator / iw::FixedTime());
			}
		}

		for (auto entity : Space->Query<Transform, CollisionObject>()) {
			auto [transform, object] = entity.Components.Tie<OtherComponents>();

			transform->Position = iw::lerp(transform->Position, object->Trans().Position, accumulator / iw::FixedTime());
			transform->Scale    = iw::lerp(transform->Scale,    object->Trans().Scale,    accumulator / iw::FixedTime());
			transform->Rotation = iw::lerp(transform->Rotation, object->Trans().Rotation, accumulator / iw::FixedTime());
		}

		accumulator += Time::DeltaTime();
	}

	void PhysicsSystem::FixedUpdate(
		EntityComponentArray& view)
	{
		//if (accumulator < iw::Time::FixedTime()) { // this if statement was here but it works better if its not there idk what the reason was lmao
			accumulator = 0;
		//}
	}

	bool PhysicsSystem::On(
		EntityDestroyedEvent& e)
	{
		CollisionObject* object = e.Entity.Find<CollisionObject>();
		if (!object) {
			object = e.Entity.Find<Rigidbody>();
		}

		if (object) {
			Physics->RemoveCollisionObject(object);
		}

		return false;
	}
}
}
