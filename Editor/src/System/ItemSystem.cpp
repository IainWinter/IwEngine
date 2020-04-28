#include "Systems/ItemSystem.h"
#include "Components/Enemy.h"
#include "Components/Player.h"
#include "Events/ActionEvents.h"

#include "iw/engine/Time.h"
#include "iw/physics/Collision/SphereCollider.h";
#include "iw/physics/Collision/CollisionObject.h";
#include "iw/audio/AudioSpaceStudio.h"
#include "iw/graphics/Model.h"

ItemSystem::ItemSystem()
	: iw::System<iw::Transform, iw::CollisionObject, Item>("Item")
{
	m_prefab.Type = NOTE;
}

int ItemSystem::Initialize() {
	iw::MeshDescription   description = Asset->Load<iw::Model>("Sphere")->GetMesh(0).Data()->Description();
	iw::ref<iw::Material> material    = Asset->Load<iw::Model>("Sphere")->GetMesh(0).Material()->MakeInstance();

	iw::Mesh note = iw::MakePlane(description, 1, 1)->MakeInstance();

	material->Set("albedo", iw::Color::From255(209, 195, 167));
	material->SetCastShadows(false);
	note.SetMaterial(material);

	m_noteMesh = note;

	return 0;
}

void ItemSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, object, circle] = entity.Components.Tie<Components>();

	}
}

bool ItemSystem::On(
	iw::ActionEvent& e)
{
	if (e.Action == iw::val(Actions::SPAWN_ITEM)) {
		SpawnItemEvent& event = e.as<SpawnItemEvent>();
		iw::Transform* note = SpawnItem(m_prefab, event.Position);
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

	iw::Entity note;
	iw::Entity other;
	if (   a != iw::EntityHandle::Empty
		&& a.Has<Item>())
	{
		note  = a;
		other = b;
	}

	else if (b != iw::EntityHandle::Empty
		&&   b.Has<Item>())
	{
		note  = b;
		other = a;
	}

	if (   other != iw::EntityHandle::Empty
		&& other.Has<Player>())
	{
		Bus->push<SpawnNoteEvent>(0);

		note.FindComponent<iw::Transform>()->SetParent(nullptr);
		Space->DestroyEntity(note.Index());
	}

	return false;
}

iw::Transform* ItemSystem::SpawnItem(
	Item prefab,
	iw::vector3 position)
{
	iw::Entity note = Space->CreateEntity<iw::Transform, iw::Mesh, iw::SphereCollider, iw::CollisionObject, Item>();

	                         note.Set<Item>(prefab);
	iw::Mesh*            m = note.Set<iw::Mesh>(m_noteMesh);
	iw::Transform*       t = note.Set<iw::Transform>(position, iw::vector3(0.65f, 0.9f, 0.9f), iw::quaternion::from_euler_angles(0, iw::Pi * 2 * rand() / RAND_MAX, 0));
	iw::SphereCollider*  s = note.Set<iw::SphereCollider>(iw::vector3::zero, 1);
	iw::CollisionObject* c = note.Set<iw::CollisionObject>();

	c->SetCol(s);
	c->SetTrans(t);
	c->SetIsTrigger(true);

	Physics->AddCollisionObject(c);

	return t;
}
