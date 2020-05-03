#include "Systems/ItemSystem.h"
#include "Events/ActionEvents.h"

#include "Components/Player.h"
#include "Components/Note.h"
#include "Components/Slowmo.h"

#include "iw/engine/Time.h"
#include "iw/physics/Collision/SphereCollider.h";
#include "iw/physics/Collision/CollisionObject.h";
#include "iw/audio/AudioSpaceStudio.h"
#include "iw/graphics/Model.h"


ItemSystem::ItemSystem()
	: iw::SystemBase("Item")
{}

int ItemSystem::Initialize() {
	iw::MeshDescription   description = Asset->Load<iw::Model>("Sphere")->GetMesh(0).Data()->Description();
	iw::ref<iw::Material> material    = Asset->Load<iw::Model>("Sphere")->GetMesh(0).Material()->MakeInstance();

	iw::Mesh note = iw::MakePlane(description, 1, 1)->MakeInstance();

	material->Set("baseColor", iw::Color::From255(209, 195, 167));
	material->SetCastShadows(false);
	note.SetMaterial(material);

	m_noteMesh = note;

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
	iw::Entity a = Space->FindEntity(e.ObjA);
	if (a == iw::EntityHandle::Empty) {
		a = Space->FindEntity<iw::Rigidbody>(e.ObjA);
	}

	iw::Entity b = Space->FindEntity(e.ObjB);
	if (b == iw::EntityHandle::Empty) {
		b = Space->FindEntity<iw::Rigidbody>(e.ObjB);
	}

	iw::Entity entity;
	iw::Entity other;
	if (   a != iw::EntityHandle::Empty
		&& a.Has<Item>())
	{
		entity = a;
		other = b;
	}

	else if (b != iw::EntityHandle::Empty
		&&   b.Has<Item>())
	{
		entity = b;
		other = a;
	}

	if (   other != iw::EntityHandle::Empty
		&& other.Has<Player>())
	{
		Item* item = entity.Find<Item>();

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

		entity.Find<iw::Transform>()->SetParent(nullptr);
		Space->DestroyEntity(entity.Index());
	}

	return false;
}

iw::Transform* ItemSystem::SpawnItem(
	Item prefab,
	iw::vector3 position)
{
	iw::Entity entity = Space->CreateEntity<iw::Transform, iw::Mesh, iw::SphereCollider, iw::CollisionObject, Item>();

	                         entity.Set<Item>(prefab);
	iw::Mesh*            m = entity.Set<iw::Mesh>(m_noteMesh);
	iw::Transform*       t = entity.Set<iw::Transform>(position, iw::vector3(0.65f, 0.9f, 0.9f), iw::quaternion::from_euler_angles(0, iw::Pi * 2 * rand() / RAND_MAX, 0));
	iw::SphereCollider*  s = entity.Set<iw::SphereCollider>(iw::vector3::zero, 1);
	iw::CollisionObject* c = entity.Set<iw::CollisionObject>();

	c->SetCol(s);
	c->SetTrans(t);
	c->SetIsTrigger(true);

	Physics->AddCollisionObject(c);

	return t;
}
