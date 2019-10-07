#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/physics/Spacial/Grid.h"

namespace IwEngine {
	PhysicsSystem::PhysicsSystem(
		IwEntity::Space& space,
		IwGraphics::RenderQueue& renderQueue)
		: IwEngine::System<Transform, IwPhysics::AABB3D>(space, renderQueue, "Physics")
	{}

	struct Components {
		IwEngine::Transform* Transform;
		IwPhysics::AABB3D* AABB;
	};

	void PhysicsSystem::Update(
		IwEntity::EntityComponentArray& view)
	{
		//IwPhysics::Grid<size_t> grid(iwm::vector3(2));
		for (auto entity : view) {
			auto [transform, aabb] = entity.Components.Tie<Components>();
			for (auto entity2 : view) {
				auto [transform2, aabb2] = entity2.Components.Tie<Components>();
				if (aabb->Intersects(*aabb2)) {
					if (entity.Index == 0) {
						QueueDestroyEntity(entity.Index);
					}
				}
			}
		}

		//LOG_INFO << grid.CellCount();
	}
}
