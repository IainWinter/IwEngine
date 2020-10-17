#include "Systems/WorldHoleSystem.h"

#include "Components/DontDeleteBullets.h"
#include "Components/WorldHole.h"
#include "Components/Player.h"
#include "Events/ActionEvents.h"

#include "iw/common/Components/Transform.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/graphics/Model.h"
#include "iw/events/binding.h"

WorldHoleSystem::WorldHoleSystem()
	: iw::SystemBase("World Hole")
	, m_active(true)
{}

int WorldHoleSystem::Initialize() {
	iw::Mesh mesh = Asset->Load<iw::Model>("Sphere")->GetMesh(0).MakeInstance();

	mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/phong.shader"));
	mesh.Material()->Set("baseColor", iw::Color(0, 0, 0, 1));
	mesh.Material()->Set("emissive", 0.0f);

	iw::Model model;
	model.AddMesh(mesh);

	iw::CollisionObject object;
	object.SetIsTrigger(true);

	iw::SphereCollider collider(0, 0.85f);

	iw::Transform transform(iw::vector3(0, 0.1f, 0), iw::vector3(5, 0.001f, 5));

	WorldHole worldHole;
	worldHole.InTransition = false;

	holePrefab.Add(Space->RegisterComponent<iw::Transform>(), &transform);
	holePrefab.Add(Space->RegisterComponent<iw::CollisionObject>(), &object);
	holePrefab.Add(Space->RegisterComponent<iw::SphereCollider>(), &collider);
	holePrefab.Add(Space->RegisterComponent<iw::Model>(), &model);
	holePrefab.Add(Space->RegisterComponent<WorldHole>(), &worldHole);
	holePrefab.Add(Space->RegisterComponent<DontDeleteBullets>());

	return 0;
}

void WorldHoleSystem::Update() {
	auto holes  = Space->Query<iw::Transform, iw::Model, WorldHole>();

	holes.Each([&](
		auto entity,
		auto transform,
		auto model,
		auto hole)
	{
		if(hole->Touched) {
			hole->Timer += iw::Time::DeltaTime();

			if (hole->Timer > hole->Time) {
				hole->Timer = hole->Time;
			}
		}

		auto material = model->GetMesh(0).Material();
		
		float color = hole->Time == 0.0f ? 0.0f : (hole->Time - hole->Timer) / hole->Time * 0.5f;

		material->Set("baseColor", iw::Color(color, color, color, 1.0f));

		//if (hole->BulletTime > 0) {

		//	hole->BulletTimer += iw::Time::DeltaTime();

		//	if (hole->BulletTimer > hole->BulletTime) {

		//		hole->BulletTimer = 0.0f;
		//		Bus->push<SpawnBulletEvent>(
		//			m_seekBullet,
		//			transform->Position,
		//			iw::quaternion::identity,
		//			transform->Parent()
		//		);
		//	}
		//}
	});

}

bool WorldHoleSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();

			if (event.LevelName == "levels/canyon/canyon02.json") {
				event.Level->AddChild(SpawnHole(0, 5, false, "levels/canyon/cave01.json"));
			}

			else if (event.LevelName == "levels/canyon/canyon03.json") {
				event.Level->AddChild(SpawnHole(iw::vector3(-8,  0, -8), 5, false, "levels/canyon/cave02.json"));
				event.Level->AddChild(SpawnHole(iw::vector3( 2,  0,  0), 5, true,  "levels/canyon/cave02.json"));
				event.Level->AddChild(SpawnHole(iw::vector3( 12, 0,  8), 5, false, "levels/canyon/cave02.json"));
			}

			else if (event.LevelName == "levels/canyon/canyon04.json") {
				event.Level->AddChild(SpawnHole(iw::vector3(4, 0, 0), 3.5f, true, "levels/canyon/cave03.json"));
			}

			else if (event.LevelName == "levels/canyon/canyon05.json") {
				event.Level->AddChild(SpawnHole(iw::vector3(0, 0, -6.5), 5, false, "levels/canyon/cave04.json"));
				event.Level->AddChild(SpawnHole(iw::vector3(0, 0,  6.5), 5, false, "levels/canyon/cave04.json"));
			}

			else if (event.LevelName == "levels/canyon/canyon07.json") {
				event.Level->AddChild(SpawnHole(iw::vector3(-5, 0, 9), 3, false, "levels/canyon/cave06.json"));
				event.Level->AddChild(SpawnHole(iw::vector3(-3, 0, 5), 3, false, "levels/canyon/cave06.json"));
				event.Level->AddChild(SpawnHole(iw::vector3(-4, 0, 6), 3, false, "levels/canyon/cave06.json"));
				event.Level->AddChild(SpawnHole(iw::vector3( 0, 0, 5), 3, false, "levels/canyon/cave06.json"));
				event.Level->AddChild(SpawnHole(iw::vector3( 3, 0, 5), 3, false, "levels/canyon/cave06.json"));
				event.Level->AddChild(SpawnHole(iw::vector3( 4, 0, 6), 3, false, "levels/canyon/cave06.json"));
				event.Level->AddChild(SpawnHole(iw::vector3( 5, 0, 9), 3, false, "levels/canyon/cave06.json"));
			}

			break;
		}
		case iw::val(Actions::LONG_DASH_ACTIVE): {
			m_active = !e.as<LongDashEvent>().Active;
			break;
		}
	}

	return false;
}

iw::Transform* WorldHoleSystem::SpawnHole(
	iw::vector3 position,
	iw::vector3 scale,
	bool crumble,
	std::string caveLevel)
{
	iw::Entity hole = Space->Instantiate(holePrefab);

	iw::Transform*       t = hole.Find<iw::Transform>();
	iw::SphereCollider*  c = hole.Find<iw::SphereCollider>();
	iw::CollisionObject* o = hole.Find<iw::CollisionObject>();
	iw::Model*           m = hole.Find<iw::Model>();
	WorldHole*           h = hole.Find<WorldHole>();

	h->CaveLevel = caveLevel;

	if (crumble) {
		h->Time = 2.0f;
		auto& mesh = m->GetMesh(0);
		auto material = mesh.Material()->MakeInstance();
		mesh.SetMaterial(material);
	}

	else {
		h->Time = 0.0f;
		h->BulletTime = 2.0f;
	}

	t->Position = position;
	t->Scale.x = scale.x;
	t->Scale.z = scale.z;

	o->SetCol(c);
	o->SetTrans(t);

	o->SetOnCollision([&](
		auto man,
		auto)
	{
		iw::Entity holeEntity, otherEntity;
		if (GetEntitiesFromManifold<WorldHole>(man, holeEntity, otherEntity)) {
			return;
		}

		if (!otherEntity.Has<Player>()) {
			return;
		}

		WorldHole* hole = holeEntity.Find<WorldHole>();

		hole->Touched = true;

		if (hole->Timer < hole->Time) {
			return;
		}

		if (   m_active
			&& !hole->InTransition)
		{
			hole->InTransition = true;

			if (hole->CaveLevel.length() == 0) {
				Bus->push<ResetLevelEvent>();
			}

			else {
				Bus->push<SetCameraTargetEvent>(holeEntity.Find<iw::Transform>()->Position, true);
				Bus->push<GotoLevelEvent>(hole->CaveLevel);
			}
		}
	});

	Physics->AddCollisionObject(o);

	return t;
}
