#include "Systems/EnemyDeathCircleSystem.h"
#include "Components/Bullet.h"
#include "Components/Enemy.h"
#include "iw/engine/Time.h"

#include "IW/physics/Collision/SphereCollider.h";
#include "IW/physics/Dynamics/Rigidbody.h";

#include "Events/ActionEvents.h"
#include "iw/audio/AudioSpaceStudio.h"

EnemyDeathCircleSystem::EnemyDeathCircleSystem()
	: iw::System<iw::Transform, iw::Model, iw::CollisionObject, EnemyDeathCircle>("Enemy Death Circle")
	, m_deathCircleModel(nullptr)
{
	m_prefab.Radius   = 4.0f;
	m_prefab.FadeTime = 2.0f;
	m_prefab.Timer    = 0.0f;
}

int EnemyDeathCircleSystem::Initialize() {
	iw::Mesh* mesh = Asset->Load<iw::Model>("Sphere")->Meshes[0].Instance();
	iw::Material mat = mesh->Material->Instance();

	iw::Model circle = { mesh, 1 };

	m_deathCircleModel = Asset->Give<iw::Model>("Death Circle", &circle);

	mat.Set("albedo", iw::Color::From255(255, 0, 0, 64));
	mat.SetTransparency(iw::Transparency::ADD);
	mat.SetCastShadows(false);

	m_deathCircleModel->Meshes[0].Material = REF<iw::Material>(mat);

	return 0;
}

void EnemyDeathCircleSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, model, object, circle] = entity.Components.Tie<Components>();

		if (circle->Timer > 2 * circle->FadeTime) {
			QueueDestroyEntity(entity.Index);
		}

		else if (circle->Timer < circle->FadeTime) {
			transform->Scale = iw::lerp(transform->Scale, iw::vector3(circle->Radius), circle->Radius / circle->FadeTime * iw::Time::DeltaTime());
			object->SetTrans(transform);
		}

		else {
			iw::Color* color = model->Meshes[0].Material->Get<iw::Color>("albedo");
			color->a = iw::lerp(color->a, 0.0f, circle->Radius / circle->FadeTime * iw::Time::DeltaTime());
		}

		circle->Timer += iw::Time::DeltaTime();
	}
}

bool EnemyDeathCircleSystem::On(
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

	if (   a == iw::EntityHandle::Empty
		|| b == iw::EntityHandle::Empty)
	{
		return false;
	}

	iw::Entity circle;
	iw::Entity bullet;
	if (   a.HasComponent<EnemyDeathCircle>()
		&& b.HasComponent<Bullet>())
	{
		circle = a;
		bullet = b;
	}

	else if (b.HasComponent<EnemyDeathCircle>()
		&&   a.HasComponent<Bullet>())
	{
		circle = b;
		bullet = a;
	}

	if (   circle != iw::EntityHandle::Empty
		&& bullet != iw::EntityHandle::Empty)
	{
		Space->DestroyEntity(bullet.Index());
	}

	return false;
}

bool EnemyDeathCircleSystem::On(
	iw::ActionEvent& e)
{
	if (e.Action == iw::val(Actions::SPAWN_ENEMY_DEATH)) {
		SpawnEnemyDeath& event = e.as<SpawnEnemyDeath>();
		SpawnDeathCircle(m_prefab, event.Position);
	}

	return false;
}

iw::Transform* EnemyDeathCircleSystem::SpawnDeathCircle(
	EnemyDeathCircle prefab,
	iw::vector3 position)
{
	iw::Entity circle = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::CollisionObject, EnemyDeathCircle>();
	
	                         circle.SetComponent<EnemyDeathCircle>   (prefab);
	iw::Model*           m = circle.SetComponent<iw::Model>          (*m_deathCircleModel);
	iw::Transform*       t = circle.SetComponent<iw::Transform>      (position, 0.0f);
	iw::SphereCollider*  s = circle.SetComponent<iw::SphereCollider> (iw::vector3::zero, 1);
	iw::CollisionObject* c = circle.SetComponent<iw::CollisionObject>();

	m->Meshes[0].Material->Set("albedo", iw::Color::From255(255, 0, 0, 64));

	c->SetCol(s);
	c->SetTrans(t);
	c->SetIsTrigger(true);

	Physics->AddCollisionObject(c);

	return t;
}
