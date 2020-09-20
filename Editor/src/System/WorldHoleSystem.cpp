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

WorldHoleSystem::WorldHoleSystem(
	iw::Entity& currentLevel)
	: iw::SystemBase("World Hole")
	, currentLevel(currentLevel)
	, m_active(true)
{}

// sucks that this needs to be here, need to fix lambdas to get it to work
void WorldHoleSystem::collide(iw::Manifold& man, iw::scalar dt)
{
	iw::Entity holeEntity, otherEntity;
	bool noent = GetEntitiesFromManifold<WorldHole>(man, holeEntity, otherEntity);

	if (noent) {
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

	if (!hole->InTransition && m_active) {
		hole->InTransition = true;

		iw::vector3 pos = holeEntity.Find<iw::Transform>()->Position;

		if (hole->CaveLevel.length() == 0) {
			Bus->push<ResetLevelEvent>();
		}

		else {
			Bus->push<SetCameraTargetEvent>(pos, true);
			Bus->push<LoadNextLevelEvent>(hole->CaveLevel);
		}
	}

	//bulletTransform->SetParent(nullptr);
	//Space->DestroyEntity(bulletEntity.Index());
}

int WorldHoleSystem::Initialize() {
	iw::Mesh mesh = Asset->Load<iw::Model>("Sphere")->GetMesh(0).MakeInstance();

	mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/phong.shader"));
	mesh.Material()->Set("baseColor", iw::Color(0, 0, 0, 1));
	mesh.Material()->Set("emissive", 0.0f);

	iw::Model model;
	model.AddMesh(mesh);

	iw::CollisionObject object;
	object.SetIsTrigger(true);

	object.SetOnCollision(iw::bind<void, WorldHoleSystem*, iw::Manifold&, float>(&WorldHoleSystem::collide, this)); // garbo

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
				SpawnHole(0, 5, false, "levels/canyon/cave01.json");
			}

			else if (event.LevelName == "levels/canyon/canyon03.json") {
				SpawnHole(iw::vector3(-8,  0, -8), 5, false, "levels/canyon/cave02.json");
				SpawnHole(iw::vector3( 2,  0,  0), 5, true,  "levels/canyon/cave02.json");
				SpawnHole(iw::vector3( 12, 0,  8), 5, false, "levels/canyon/cave02.json");
			}

			else if (event.LevelName == "levels/canyon/canyon04.json") {
				SpawnHole(iw::vector3(4, 0, 0), 3.5f, true, "levels/canyon/cave03.json");
			}

			else if (event.LevelName == "levels/canyon/canyon05.json") {
				SpawnHole(iw::vector3(0, 0, -6.5), 5, false, "levels/canyon/cave04.json");
				SpawnHole(iw::vector3(0, 0,  6.5), 5, false, "levels/canyon/cave04.json");
			}

			else if (event.LevelName == "levels/canyon/canyon07.json") {
				SpawnHole(iw::vector3(0, 0, 6), 5, false, "levels/canyon/cave06.json");
			}

			else if (event.LevelName == "levels/canyon/cave06.json") {
				SpawnHole(iw::vector3(-17.2f, 0, 0), 4, true, "");
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

void WorldHoleSystem::SpawnHole(
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

	if (currentLevel) {
		t->SetParent(currentLevel.Find<iw::Transform>());
	}

	o->SetCol(c);
	o->SetTrans(t);
	Physics->AddCollisionObject(o);
}
