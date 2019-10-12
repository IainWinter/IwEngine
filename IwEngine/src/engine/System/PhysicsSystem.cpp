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

	//float t = 0;

	void PhysicsSystem::Update(
		IwEntity::EntityComponentArray& eca)
	{
		//IwPhysics::Grid<size_t, iwm::vector2> grid(iwm::vector2(10));
		for (auto entityA : eca) {
			auto [transformA, aabbA] = entityA.Components.Tie<Components>();
			for (auto entityB : eca) {
				auto [transformB, aabbB] = entityB.Components.Tie<Components>();

				IwPhysics::AABB2D a = {
					aabbA->Min + transformA->Position,
					aabbB->Max + transformA->Position
				};

				IwPhysics::AABB2D b = {
					aabbA->Min + transformA->Position,
					aabbB->Max + transformA->Position
				};

				if (a.Intersects(b)) {
					
					
					//LOG_INFO << "Intersects";
				}
			}
			//grid.Insert(entity.Index, transform->Position, *aabb);
		}

		//for (auto entity : Space.Query<Transform, IwPhysics::AABB2D>()) {
		//	auto [transform, aabb] = entity.Components.Tie<Components>();
		//	grid.FindPairs(entity.Index, *transform, *aabb);

		//}

		/*t += Time::DeltaTime();

		if (t > .1f) {
			t = 0;
			LOG_INFO << grid.CellCount();
		}*/
	}
}
