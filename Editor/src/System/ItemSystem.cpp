#include "Systems/ItemSystem.h"
#include "Events/ActionEvents.h"

#include "Components/Player.h"
#include "Components/Note.h"

#include "iw/engine/Time.h"
#include "iw/physics/Collision/SphereCollider.h";
#include "iw/physics/Collision/CollisionObject.h";
#include "iw/audio/AudioSpaceStudio.h"



ItemSystem::ItemSystem()
	: iw::SystemBase("Item")
{}

int ItemSystem::Initialize() {
	iw::MeshDescription   description = Asset->Load<iw::Model>("Sphere")->GetMesh(0).Data()->Description();
	iw::ref<iw::Material> material = Asset->Load<iw::Material>("materials/Default")->MakeInstance();

	iw::Mesh note = iw::MakePlane(description, 1, 1)->MakeInstance();

	//material->Set("baseColor", iw::Color::From255(209, 195, 167));
	//material->Set("emissive", 1.2f);
	//material->Set("reflectance", 1.0f);

	material->Set("reflectance", 0.0f);
	material->Set("emissive", 1.0f);
	material->Set("indirectDiffuse", 1);
	material->Set("indirectSpecular", 1);

	//material->SetCastShadows(false);
	note.SetMaterial(material);

	m_noteModel.AddMesh(note);

	return 0;
}

bool ItemSystem::On(
	iw::ActionEvent& e)
{
	if (e.Action == iw::val(Actions::SPAWN_ITEM)) {
		SpawnItemEvent& event = e.as<SpawnItemEvent>();
		iw::Transform* note = SpawnItem(event.Item, event.Position);
		note->SetParent(event.Level);
	}

	return false;
}

bool ItemSystem::On(
	iw::CollisionEvent& e)
{
	iw::Entity itemEntity, otherEntity;
	bool noent = GetEntitiesFromManifold<Item>(e.Manifold, itemEntity, otherEntity);
	
	if (noent) {
		return false;
	}

	if (otherEntity.Has<Player>()) {
		Item* item = itemEntity.Find<Item>();

		switch (item->Type) {
			case NOTE: {
				Bus->push<SpawnNoteEvent>(item->Id);
				break;
			}
			case CONSUMABLE: {
				Bus->push<SpawnConsumableEvent>(item->Id);
				break;
			}
			default: {
				LOG_WARNING << "Tried to spawn item with an invalid type";
				break;
			}
		}

		//Bus->push<iw::EntityDestroyEvent>(itemEntity.Handle);

		itemEntity.Find<iw::Transform>()->SetParent(nullptr);
		Space->DestroyEntity(itemEntity.Index());
	}

	return false;
}

iw::Transform* ItemSystem::SpawnItem(
	Item prefab,
	iw::vector3 position)
{
	iw::Entity entity = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::CollisionObject, Item>();

	                         entity.Set<Item>(prefab);
	                         entity.Set<iw::Model>(m_noteModel);
	iw::Transform*       t = entity.Set<iw::Transform>(position, iw::vector3(0.65f, 0.9f, 0.9f), iw::quaternion::from_euler_angles(0, iw::Pi * 2 * rand() / RAND_MAX, 0));
	iw::SphereCollider*  s = entity.Set<iw::SphereCollider>(iw::vector3::zero, 1);
	iw::CollisionObject* c = entity.Set<iw::CollisionObject>();

	c->SetCol(s);
	c->SetTrans(t);
	c->SetIsTrigger(true);

	Physics->AddCollisionObject(c);

	return t;
}
