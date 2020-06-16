#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/engine/Time.h"

#include "iw/graphics/Mesh.h"

#include "iw/graphics/ParticleSystem.h"

namespace iw {
namespace Engine {
	EntityCleanupSystem::EntityCleanupSystem()
		: System<>("Entity Cleanup")
	{}

	bool EntityCleanupSystem::On(
		EntityDestroyEvent& e)
	{
		Space->DestroyEntity(e.Entity.Index);
		return true;
	}
	
	bool EntityCleanupSystem::On(
		EntityDestroyedEvent& e)
	{
		Transform* transform = Space->FindComponent<Transform>(e.Entity);
		if (transform) {
			//transform->SetParent(nullptr); //causes problems for some reason
		}

		Mesh* mesh = Space->FindComponent<Mesh>(e.Entity);
		if (mesh) {
			mesh->Data()    .reset();
			mesh->Material().reset();
		}

		ParticleSystem<StaticParticle>* sys = Space->FindComponent<ParticleSystem<StaticParticle>>(e.Entity);
		if (sys) {
			sys->~ParticleSystem();
			sys->GetParticleMesh().Data()    .reset();
			sys->GetParticleMesh().Material().reset();
		}

		return false;
	}
}
}
