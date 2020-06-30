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

	if (!hole->InTransition) {
		hole->InTransition = true;

		iw::vector3 pos = holeEntity.Find<iw::Transform>()->Position;

		Task->queue([=]() {
			Bus->push<SetCameraTargetEvent>(pos, true);
			Bus->push<LoadNextLevelEvent>("levels/canyon/cave01.json");
		});
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

	iw::Rigidbody rigidbody;
	rigidbody.SetMass(1);
	rigidbody.SetSimGravity(false);
	rigidbody.SetIsTrigger(true);

	rigidbody.SetOnCollision(iw::bind<void, WorldHoleSystem*, iw::Manifold&, float>(&WorldHoleSystem::collide, this)); // garbo

	iw::SphereCollider collider(0, 0.9f);

	iw::Transform transform(iw::vector3(0, 0.1f, 0), iw::vector3(5, 0.001f, 5));

	WorldHole worldHole;
	worldHole.InTransition = false;

	holePrefab.Add(Space->RegisterComponent<iw::Transform>(), &transform);
	holePrefab.Add(Space->RegisterComponent<iw::Rigidbody>(), &rigidbody);
	holePrefab.Add(Space->RegisterComponent<iw::SphereCollider>(), &collider);
	holePrefab.Add(Space->RegisterComponent<iw::Model>(), &model);
	holePrefab.Add(Space->RegisterComponent<WorldHole>(), &worldHole);
	holePrefab.Add(Space->RegisterComponent<DontDeleteBullets>());

	return 0;
}

void WorldHoleSystem::FixedUpdate() {
	auto bullets  = Space->Query<iw::Transform, iw::Rigidbody, Bullet>();
}

bool WorldHoleSystem::On(
	iw::ActionEvent& e)
{
	if (e.Action == iw::val(Actions::START_LEVEL)) {
		StartLevelEvent& event = e.as<StartLevelEvent>();

		if (event.LevelName == "levels/canyon/canyon01.json") {
			SpawnHole(0);
		}

		if (event.LevelName == "levels/canyon/canyon02.json") {
			SpawnHole(iw::vector3(8, 0, 6));
			SpawnHole(iw::vector3(0, 0, -6));
		}
	}

	return false;
}

void WorldHoleSystem::SpawnHole(
	iw::vector3 position)
{
	iw::Entity hole = Space->Instantiate(holePrefab);

	iw::Transform*      t = hole.Find<iw::Transform>();
	iw::SphereCollider* c = hole.Find<iw::SphereCollider>();
	iw::Rigidbody*      r = hole.Find<iw::Rigidbody>();

	t->Position = position;

	if (currentLevel) {
		t->SetParent(currentLevel.Find<iw::Transform>());
	}

	r->SetCol(c);
	r->SetTrans(t);
	Physics->AddRigidbody(r);
}
