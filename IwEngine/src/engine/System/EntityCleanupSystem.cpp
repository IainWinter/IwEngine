#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/engine/Time.h"

#include "iw/graphics/Mesh.h"

namespace iw {
namespace Engine {
	EntityCleanupSystem::EntityCleanupSystem()
		: System<>("Entity Cleanup")
	{}
	
	bool EntityCleanupSystem::On(
		EntityDestroyedEvent& e)
	{
		Transform* transform = e.Entity.Find<Transform>();
		if (transform) {
			//transform->SetParent(nullptr); //causes problems for some reason
		}

		Mesh* mesh = e.Entity.Find<Mesh>();
		if (mesh) {
			mesh->Data()    .reset();
			mesh->Material().reset();
		}

		return false;
	}
}
}
