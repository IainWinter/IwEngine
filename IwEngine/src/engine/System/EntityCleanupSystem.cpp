#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/engine/Time.h"

#include "iw/graphics/Model.h"

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
			//for (iw::Transform* child : transform->Children()) {
			//	Space->QueueEntity(Space->FindEntity(child).Handle, func_Destroy);
			//}

			transform->SetParent(nullptr); //causes problems for some reason
		}

		Mesh* mesh = Space->FindComponent<Mesh>(e.Entity);
		if (mesh) {
			mesh->Data()    .reset();
			mesh->Material().reset();
		}

		Model* model = Space->FindComponent<Model>(e.Entity);
		if (model) {
			model->~Model();
		}

		ParticleSystem<StaticParticle>* sys = Space->FindComponent<ParticleSystem<StaticParticle>>(e.Entity);
		if (sys) {
			sys->~ParticleSystem();
			//sys->GetParticleMesh().Data()    .reset();
			//sys->GetParticleMesh().Material().reset();
		}

		iw::Rigidbody* body = Space->FindComponent<iw::Rigidbody>(e.Entity);
		if (body) {
			Physics->RemoveCollisionObject(body);
		}

		iw::CollisionObject* obj = Space->FindComponent<iw::CollisionObject>(e.Entity);
		if (obj) {
			Physics->RemoveCollisionObject(obj);
		}

		return false;
	}
}
}
