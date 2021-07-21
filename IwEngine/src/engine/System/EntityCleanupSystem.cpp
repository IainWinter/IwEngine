#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/engine/Time.h"

#include "iw/graphics/Model.h"

#include "iw/graphics/ParticleSystem.h"

namespace iw {
namespace Engine {
	EntityCleanupSystem::EntityCleanupSystem()
		: SystemBase("Entity Cleanup")
	{}

	// need to add a event for when component data is moved so the components that use pointers can be
	// repointed at the correct thing, ie Rigidbody -> Collider*

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
			//for (Transform* child : transform->Children()) {
			//	Space->QueueEntity(Space->FindEntity(child).Handle, func_Destroy);
			//}

			transform->SetParent(nullptr); //causes problems for some reason
		}

		Mesh* mesh = Space->FindComponent<Mesh>(e.Entity);
		if (mesh) {
			mesh->Data    .reset();
			mesh->Material.reset();
		}

		Model* model = Space->FindComponent<Model>(e.Entity);
		if (model) {
			model->~Model();
		}

		ParticleSystem<StaticParticle>* sys = Space->FindComponent<ParticleSystem<StaticParticle>>(e.Entity);
		if (sys) {
			sys->~ParticleSystem();
			//sys->GetParticleMesh().Data()    .reset();
			//sys->GetParticleMesh().Material.reset();
		}

		Rigidbody* body = Space->FindComponent<Rigidbody>(e.Entity);
		if (body) {
			Physics->RemoveCollisionObject(body);
		}

		CollisionObject* obj = Space->FindComponent<CollisionObject>(e.Entity);
		if (obj) {
			Physics->RemoveCollisionObject(obj);
		}

		return false;
	}

	bool EntityCleanupSystem::On(
		EntityMovedEvent& e) 
	{
		LOG_WARNING << "Entity " << e.Entity.Index << "'s data has moved!";
		// Physics components
		//
		//CollisionObject* obj = Space->FindComponent<Rigidbody>(e.Entity);
		//if (!obj) {
		//	obj = Space->FindComponent<CollisionObject>(e.Entity);
		//}

		//if (obj) {
		//	Physics->RemoveCollisionObject(obj);



		//}

		return false;
	}
}
}
