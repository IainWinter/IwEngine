#include "iw/engine/Systems/PhysicsSystem.h"

namespace iw {
namespace Engine {
	PhysicsSystem::PhysicsSystem()
		: System<Rigidbody>("Physics")
	{}

	void PhysicsSystem::Update(
		EntityComponentArray& eca)
	{

		// Loop through every 

		//IwPhysics::Grid<size_t, iw::vector2> grid(iw::vector2(10));

		//for (auto entity : eca) {
		//	if (entity.Index == 1) {
		//		for (auto entity2 : eca) {
		//			if (entity2.Index == 1) continue;

		//			auto [transform1, aabb1] = entity.Components.Tie<Components>();
		//			auto [transform2, aabb2] = entity2.Components.Tie<Components>();

		//			if (IwPhysics::AABB2(transform1->Position + aabb1->Min, transform1->Position + aabb1->Max).Intersects(
		//				IwPhysics::AABB2(transform2->Position + aabb2->Min, transform2->Position + aabb2->Max)))
		//			{
		//				LOG_INFO << entity.Index << " colliding with " << entity2.Index;
		//				QueueDestroyEntity(entity2.Index);
		//			}
		//		}
		//	}
		//}
	}

	bool PhysicsSystem::On(
		EntityDestroyedEvent& e)
	{
		if (e.Entity.HasComponent<Rigidbody>()) {
			Rigidbody* body = e.Entity.FindComponent<Rigidbody>();
			Physics->RemoveRigidbody(body);
		}

		return false;
	}
}
}
