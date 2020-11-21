#include "Systems/CircleEffectSystem.h"
#include "Components/Bullet.h"
#include "Components/Enemy.h"
#include "iw/engine/Time.h"

#include "IW/physics/Collision/SphereCollider.h";
#include "IW/physics/Dynamics/Rigidbody.h";

#include "Events/ActionEvents.h"
#include "iw/audio/AudioSpaceStudio.h"

CircleEffectSystem::CircleEffectSystem()
	: iw::SystemBase("Circle Effect")
{
	m_charged = false;
}

int CircleEffectSystem::Initialize() {
	iw::Mesh sphere = Asset->Load<iw::Model>("Sphere")->GetMesh(0).MakeInstance();

	sphere.Material()->Set("baseColor", iw::Color::From255(0, 195, 255, 64));
	sphere.Material()->Set("emissive", 5.0f);
	sphere.Material()->SetTransparency(iw::Transparency::ADD);
	sphere.Material()->SetCastShadows(false);

	iw::Model model;
	model.AddMesh(sphere);

	iw::Transform transform(0, 0);
	iw::SphereCollider collider(0, 1);
	iw::CollisionObject object;

	object.SetIsTrigger(true);

	EffectCircle circle;

	m_effectCirclePrefab.Add(Space->RegisterComponent<iw::Transform>(),       &transform);
	m_effectCirclePrefab.Add(Space->RegisterComponent<iw::SphereCollider>(),  &collider);
	m_effectCirclePrefab.Add(Space->RegisterComponent<iw::CollisionObject>(), &object);
	m_effectCirclePrefab.Add(Space->RegisterComponent<iw::Model>(),           &model);
	m_effectCirclePrefab.Add(Space->RegisterComponent<EffectCircle>(), &circle);

	// Everything else is defaulted in struct

	m_bulletReversal.Effect = CircleEffects::BULLET_REVERSAL;

	return 0;
}

void CircleEffectSystem::Update() {
	auto circles = Space->Query<iw::Transform, iw::Model, iw::CollisionObject, EffectCircle>();

	for (auto entity : circles) {
		auto [transform, model, object, circle] = entity.Components.Tie<Components>();

		float rate = circle->Radius / circle->FadeTime * iw::Time::DeltaTime();

		if (circle->Timer > 2 * circle->FadeTime) {
			transform->SetParent(nullptr);
			Space->QueueEntity(entity.Handle, iw::func_Destroy);
		}

		else if (circle->Timer < circle->FadeTime) {
			transform->Scale = iw::lerp(transform->Scale, iw::vector3(circle->Radius), rate);
			object->SetTrans(transform);
		}

		else {
			iw::Color* color = model->GetMesh(0).Material()->Get<iw::Color>("baseColor");
			color->a = iw::lerp(color->a, 0.0f, rate);

			if (object->Col()) {
				object->SetCol(nullptr);
			}
		}

		circle->Timer += iw::Time::DeltaTime();
	}
}

bool CircleEffectSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::SPAWN_ENEMY_DEATH): {
			SpawnEnemyDeath& event = e.as<SpawnEnemyDeath>();
			iw::Transform* circle = SpawnEffectCircle(m_deathCircle, event.Position);
			circle->SetParent(event.Level);
			break;
		}
		case iw::val(Actions::CHARGE_KILL_ACTIVE): {
			ChargeKillEvent& event = e.as<ChargeKillEvent>();
			m_charged = event.Timer > 0.0f;
			break;
		}
		case iw::val(Actions::BULLET_REVERSAL_ACTIVE): {
			BulletReversalEvent& event = e.as<BulletReversalEvent>();
			iw::Transform* circle = SpawnEffectCircle(m_bulletReversal, event.Position);
			circle->SetParent(event.Parent);
			break;
		}
	}

	return false;
}

iw::Transform* CircleEffectSystem::SpawnEffectCircle(
	EffectCircle circle,
	iw::vector3 position)
{
	iw::Entity entity = Space->Instantiate(m_effectCirclePrefab);

	iw::Transform*       t = entity.Find<iw::Transform>();
	iw::SphereCollider*  s = entity.Find<iw::SphereCollider>();
	iw::CollisionObject* o = entity.Find<iw::CollisionObject>();
	t->Position = position;
	o->SetCol(s);
	o->SetTrans(t);
	Physics->AddCollisionObject(o);

	EffectCircle* c = entity.Set<EffectCircle>(circle);
	if (circle.Effect == CircleEffects::ENEMY_DEATH_CIRCLE) {
		c->Radius   = m_charged ? 15.0f : c->Radius;
		c->FadeTime = m_charged ?  2.0f : c->FadeTime;
	}

	iw::Model* m = entity.Find<iw::Model>();
	
	iw::ref<iw::Material> material = m->GetMesh(0).Material()->MakeInstance();
	if (circle.Effect == CircleEffects::BULLET_REVERSAL) {
		material->Set("baseColor", iw::Color::From255(255, 54, 54, 64));
	}
	m->GetMesh(0).SetMaterial(material);

	return t;
}
