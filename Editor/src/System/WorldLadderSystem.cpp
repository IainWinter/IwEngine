#include "Systems/WorldLadderSystem.h"

#include "Components/WorldLadder.h"
#include "Components/Player.h"
#include "Events/ActionEvents.h"

#include "iw/common/Components/Transform.h"
#include "iw/graphics/Model.h"
#include "Components/LevelDoor.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Collision/SphereCollider.h"

WorldLadderSystem::WorldLadderSystem(
	iw::Entity& currentLevel,
	GameSaveState* saveState)
	: iw::SystemBase("World Ladder")
	, currentLevel(currentLevel)
	, saveState(saveState)
{}

int WorldLadderSystem::Initialize() {
	iw::ref<iw::Model> model = Asset->Load<iw::Model>("models/ladder.gltf");

	for (iw::Mesh& mesh : model->GetMeshes()) {
		mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/phong.shader"));
	}

	ladderPrefab.Add(Space->RegisterComponent<iw::Transform>());
	ladderPrefab.Add(Space->RegisterComponent<iw::Model>(), model.get());
	ladderPrefab.Add(Space->RegisterComponent<WorldLadder>());
	ladderPrefab.Add(Space->RegisterComponent<iw::CollisionObject>());
	ladderPrefab.Add(Space->RegisterComponent<iw::SphereCollider>());

	return 0;
}

void WorldLadderSystem::Update() {
	//auto ladders = Space->Query<iw::Transform, iw::Model, WorldLadder>();

	//ladders.Each([&](
	//	auto entity,
	//	auto transform,
	//	auto model,
	//	auto hole)
	//{
	//	if(hole->Touched) {
	//		hole->Timer += iw::Time::DeltaTime();

	//		if (hole->Timer > hole->Time) {
	//			hole->Timer = hole->Time;
	//		}
	//	}

	//	auto material = model->GetMesh(0).Material();
	//	
	//	float color = hole->Time == 0.0f ? 0.0f : (hole->Time - hole->Timer) / hole->Time * 0.5f;

	//	material->Set("baseColor", iw::Color(color, color, color, 1.0f));

	//	//if (hole->BulletTime > 0) {

	//	//	hole->BulletTimer += iw::Time::DeltaTime();

	//	//	if (hole->BulletTimer > hole->BulletTime) {

	//	//		hole->BulletTimer = 0.0f;
	//	//		Bus->push<SpawnBulletEvent>(
	//	//			m_seekBullet,
	//	//			transform->Position,
	//	//			iw::quaternion::identity,
	//	//			transform->Parent()
	//	//		);
	//	//	}
	//	//}
	//});
}

bool WorldLadderSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();

			if (event.LevelName == "levels/canyon/cave03.a.json") {
				SpawnLadder(iw::vector3(15, 0, 4), iw::quaternion::from_euler_angles(0, -0.4f, 0), saveState->Cave03LadderDown, false, "levels/canyon/canyon04.a.json");
			}

			if (event.LevelName == "levels/canyon/canyon04.a.json") {
				SpawnLadder(iw::vector3(15, 0, -4), iw::quaternion::from_euler_angles(0, -0.4f, 0), saveState->Cave03LadderDown, true, "levels/canyon/cave03.a.json");
			}

			else if (event.LevelName == "levels/canyon/cave06.json") {
				SpawnLadder(iw::vector3(3.5f, 0, -14), iw::quaternion::from_euler_angles(0, 0.7f, 0), saveState->Cave06LadderDown, false, "levels/canyon/canyon07.a.json");
			}

			break;
		}
	}

	return false;
}

void WorldLadderSystem::SpawnLadder(
	iw::vector3 position,
	iw::quaternion rotation,
	bool down,
	bool above,
	std::string level)
{
	iw::Entity ladder = Space->Instantiate(ladderPrefab);

	iw::Transform* t = ladder.Find<iw::Transform>();
	iw::Model*     m = ladder.Find<iw::Model>();
	WorldLadder*   l = ladder.Find<WorldLadder>();
	iw::CollisionObject* o = ladder.Find<iw::CollisionObject>();
	iw::SphereCollider*  s = ladder.Find<iw::SphereCollider>();

	t->Position = position;
	t->Rotation = rotation;

	l->Level = level;
	l->Open = down;

	t->SetParent(currentLevel.Find<iw::Transform>());

	s->Radius = 1;
	o->SetTrans(t);
	o->SetCol(s);
	o->SetIsTrigger(true);
	o->SetOnCollision([&](iw::Manifold& man, float dt) {
		iw::Entity ladder, player;
		if (GetEntitiesFromManifold<WorldLadder, Player>(man, ladder, player)) {
			return;
		}

		Bus->send<LoadNextLevelEvent>(ladder.Find<WorldLadder>()->Level);
	});

	Physics->AddCollisionObject(o);

	if (above) {

	}

	else {
		if (down) {
			//m->GetTransform(0).Position.y = 10.0f;
		}
	}
}
