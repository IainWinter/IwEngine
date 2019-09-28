#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/physics/Spacial/Grid.h"

namespace IwEngine {
	PhysicsSystem::PhysicsSystem(
		IwEntity::Space& space,
		IwGraphics::RenderQueue& renderQueue)
		: IwEngine::System<Transform, IwPhysics::AABB3D>(space, renderQueue, "Physics")
	{}

	void PhysicsSystem::Update(
		View& view)
	{
		IwPhysics::Grid<IwEntity::Entity> grid(iwm::vector3(2));
		//for (auto components : view) {
		//	auto transform = components.GetComponent<Transform>();
		//	//auto aabb      = components.GetComponent<IwPhysics::AABB3D>();

		//	//grid.Insert(components.Entity, transform, aabb);
		//}
	}
}
