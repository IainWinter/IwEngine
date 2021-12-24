#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Time.h"

namespace iw {
namespace Engine {
	PhysicsSystem::PhysicsSystem()
		: System<Transform, Rigidbody>("Physics")
		, accumulator(0.0f)
	{}

	struct OtherComponents {
		Transform* Transform;
		CollisionObject* Object;
	};

	void PhysicsSystem::Update(
		EntityComponentArray& eca)
	{
		accumulator += Time::DeltaTime();

		float a = glm::clamp(accumulator / iw::RawFixedTime(), 0.f, 1.f);

		for (auto entity : eca) {
			auto [transform, rigidbody] = entity.Components.Tie<Components>();
			
			//if (rigidbody->IsKinematic) {
				transform->Position = iw :: lerp(rigidbody->LastTrans().Position, rigidbody->Transform.Position, a);
				transform->Scale    = iw :: lerp(rigidbody->LastTrans().Scale,    rigidbody->Transform.Scale,    a);
				transform->Rotation = glm::slerp(rigidbody->LastTrans().Rotation, rigidbody->Transform.Rotation, a);
			//}
		}

		for (auto entity : Space->Query<Transform, CollisionObject>()) {
			auto [transform, object] = entity.Components.Tie<OtherComponents>();

			transform->Position = object->Transform.Position;
			transform->Scale    = object->Transform.Scale;
			transform->Rotation = object->Transform.Rotation;

			transform->Position = iw :: lerp(transform->Position, object->Transform.Position, a);
			transform->Scale    = iw :: lerp(transform->Scale,    object->Transform.Scale,    a);
			transform->Rotation = glm::slerp(transform->Rotation, object->Transform.Rotation, a);

			//object->Trans().SetParent(transform->Parent());
		}
	}

	void PhysicsSystem::FixedUpdate(
		EntityComponentArray& view)
	{
		//if (accumulator > iw::Time::FixedTime()) { // this if statement was here but it works better if its not there idk what the reason was lmao
			accumulator = 0;
		//}
	}
}
}
