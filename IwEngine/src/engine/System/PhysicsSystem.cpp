#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/physics/Spacial/Grid.h"
#include "iw/engine/Time.h"

namespace IwEngine {
	PhysicsSystem::PhysicsSystem(
		IwEntity::Space& space,
		IwGraphics::RenderQueue& renderQueue)
		: IwEngine::System<Transform, IwPhysics::AABB2D>(space, renderQueue, "Physics")
	{}

	struct Components {
		Transform* Transform;
		IwPhysics::AABB2D* AABB;
	};

	void PhysicsSystem::Update(
		IwEntity::EntityComponentArray& eca)
	{
		IwPhysics::Grid<size_t, iwm::vector2> grid(iwm::vector2(10));
		for (auto entity : eca) {
			auto [transform, aabb] = entity.Components.Tie<Components>();
			grid.Insert(entity.Index, transform->Position, *aabb);
		}

		//for (auto entity : eca) {
		//	auto [transform, aabb] = entity.Components.Tie<Components>();
		//	grid.FindPairs(entity.Index, transform->Position, *aabb);
		//}
	}
}
