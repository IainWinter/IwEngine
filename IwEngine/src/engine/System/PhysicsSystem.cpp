#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/physics/Spacial/Grid.h"
#include "iw/engine/Time.h"

namespace IwEngine {
	PhysicsSystem::PhysicsSystem(
		IwEntity::Space& space,
		IW::Graphics::Renderer& renderer)
		: IwEngine::System<IW::Transform, IwPhysics::AABB2D>(space, renderer, "Physics")
	{}

	struct Components {
		IW::Transform* Transform;
		IwPhysics::AABB2D* AABB;
	};

	void PhysicsSystem::Update(
		IwEntity::EntityComponentArray& eca)
	{



		//IwPhysics::Grid<size_t, iwm::vector2> grid(iwm::vector2(10));

		//for (auto entity : eca) {
		//	if (entity.Index == 1) {
		//		for (auto entity2 : eca) {
		//			if (entity2.Index == 1) continue;

		//			auto [transform1, aabb1] = entity.Components.Tie<Components>();
		//			auto [transform2, aabb2] = entity2.Components.Tie<Components>();

		//			if (IwPhysics::AABB2D(transform1->Position + aabb1->Min, transform1->Position + aabb1->Max).Intersects(
		//				IwPhysics::AABB2D(transform2->Position + aabb2->Min, transform2->Position + aabb2->Max)))
		//			{
		//				LOG_INFO << entity.Index << " colliding with " << entity2.Index;
		//				QueueDestroyEntity(entity2.Index);
		//			}
		//		}
		//	}
		//}
	}
}
