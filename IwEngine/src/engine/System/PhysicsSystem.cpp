#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Time.h"

#include "glm/gtx/compatibility.hpp"

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

		float a = glm::clamp(accumulator / iw::FixedTime(), 0.f, 1.f);


		for (auto entity : eca) {
			auto [transform, rigidbody] = entity.Components.Tie<Components>();
			
			transform->Position = rigidbody->Trans().Position;
			transform->Scale    = rigidbody->Trans().Scale;
			transform->Rotation = rigidbody->Trans().Rotation;

			//if (rigidbody->IsKinematic()) {
				//transform->Position = glm::lerp(rigidbody->LastTrans().Position,  rigidbody->Trans().Position, a);
				//transform->Scale    = glm::lerp(rigidbody->LastTrans().Scale,     rigidbody->Trans().Scale,    a);
				//transform->Rotation = glm::slerp(rigidbody->LastTrans().Rotation, rigidbody->Trans().Rotation, a);
			//}
		}

		for (auto entity : Space->Query<Transform, CollisionObject>()) {
			auto [transform, object] = entity.Components.Tie<OtherComponents>();

			transform->Position = object->Trans().Position;
			transform->Scale    = object->Trans().Scale;
			transform->Rotation = object->Trans().Rotation;

			//transform->Position = glm::lerp(transform->Position, object->Trans().Position, a);
			//transform->Scale    = glm::lerp(transform->Scale,    object->Trans().Scale,    a);
			//transform->Rotation = glm::slerp(transform->Rotation, object->Trans().Rotation, a);

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
