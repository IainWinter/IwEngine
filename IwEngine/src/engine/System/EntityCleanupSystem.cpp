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
		Space->DestroyEntity(e.Entity);
		return true;
	}
	
	bool EntityCleanupSystem::On(
		EntityDestroyedEvent& e)
	{
		if (auto component = Space->GetComponent<Transform>())
		{
			Transform* transform = (Transform*)Space->FindComponent(e.Entity, component);
			if (transform) {
				//for (Transform* child : transform->Children()) {
				//	Space->QueueEntity(Space->FindEntity(child).Handle, func_Destroy);
				//}

				transform->SetParent(nullptr); //causes problems for some reason
			}
		}

		if (auto component = Space->GetComponent<Mesh>()) 
		{
			Mesh* mesh = (Mesh*)Space->FindComponent(e.Entity, component);
			if (mesh) {
				mesh->Data    .reset();
				mesh->Material.reset();
			}
		}

		if (auto component = Space->GetComponent<Model>())
		{
			Model* model = (Model*)Space->FindComponent(e.Entity, component);
			if (model) {
				model->~Model();
			}
		}

		if (auto component = Space->GetComponent<ParticleSystem<StaticParticle>>())
		{
			ParticleSystem<StaticParticle>* sys = (ParticleSystem<StaticParticle>*)Space->FindComponent(e.Entity, component);
			if (sys) {
				sys->~ParticleSystem();
				//sys->GetParticleMesh().Data()    .reset();
				//sys->GetParticleMesh().Material.reset();
			}
		}

		if (auto component = Space->GetComponent<Rigidbody>())
		{
			Rigidbody* body = (Rigidbody*)Space->FindComponent(e.Entity, component);
			if (body) {
				Physics->RemoveCollisionObject(body);
			}
		}


		if (auto component = Space->GetComponent<CollisionObject>())
		{
			CollisionObject* obj = (CollisionObject*)Space->FindComponent(e.Entity, component);
			if (obj) {
				Physics->RemoveCollisionObject(obj);
			}
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
