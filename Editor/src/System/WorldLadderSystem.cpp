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
	GameSaveState* saveState)
	: iw::SystemBase("World Ladder")
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
	auto ladders = Space->Query<iw::Transform, iw::Model, WorldLadder>();

	ladders.Each([&](
		auto entity,
		auto transform,
		auto model,
		auto ladder)
	{
		if(ladder->Transition) {
			if (ladder->Above) {
				iw::vector3& a = model->GetTransform(0).Position;
				iw::vector3& b = model->GetTransform(1).Position;
				iw::vector3& c = model->GetTransform(2).Position;
				iw::vector3& d = model->GetTransform(3).Position;
				
				float delta = iw::DeltaTime() * ladder->Speed;

				a.y -= delta;

				if (a.y < -5) {
					b.y -= delta;
				}

				if (b.y < -5) {
					c.y -= delta;
				}

				if (c.y < -5) {
					ladder->Transition = false;
					ladder->Open = !ladder->Open;
					*ladder->SaveState = true;
				}

				ladder->Speed += iw::DeltaTime() * 9.81;
			}
		}
	});
}

bool WorldLadderSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();

			currentLevel = Space->FindEntity(event.Level); // pass to functions

			LOG_INFO << "Welcome to level '" << event.LevelName << "'";

			if (event.LevelName == "levels/canyon/cave03.a.json") {
				SpawnLadder(iw::vector3(15, 0, 4), iw::quaternion::from_euler_angles(0, -0.4f, 0), &saveState->Cave03LadderDown, false, "levels/canyon/canyon04.a.json");
			}

			if (event.LevelName == "levels/canyon/canyon04.a.json") {
				SpawnLadder(iw::vector3(-2, 0, -4), iw::quaternion::from_euler_angles(0, -0.4f, 0), &saveState->Cave03LadderDown, true, "levels/canyon/cave03.a.json");
			}

			else if (event.LevelName == "levels/canyon/cave06.json") {
				SpawnLadder(iw::vector3(3.5f, 0, -14), iw::quaternion::from_euler_angles(0, 0.7f, 0), &saveState->Cave06LadderDown, false, "levels/canyon/canyon07.a.json");
			}

			else if (event.LevelName == "levels/canyon/cave08.json") {
				SpawnLadder(iw::vector3(8, 0, 0), iw::quaternion::from_euler_angles(0, 1.57f, 0), nullptr, false, "levels/canyon/top01.json");
			}

			else if (event.LevelName == "levels/canyon/canyon07.a.json") {
				SpawnLadder(iw::vector3(2, 0, -5), iw::quaternion::from_euler_angles(0, 0.7f, 0), &saveState->Cave06LadderDown, true, "levels/canyon/cave06.json");
			}

			else if (event.LevelName == "levels/canyon/top08.json") {
				iw::Transform* t = SpawnLadder(iw::vector3(15, -5, 14), iw::quaternion::from_euler_angles(0.3f, -0.2f, 0), &saveState->Top08LadderDown, true, "levels/canyon/canyon02.json");
				Space->FindEntity(t).Find<iw::SphereCollider>()->Center = iw::vector3(0, 5.8f, -3.5f); // position collider higher up
			}

			else if (event.LevelName == "levels/canyon/canyon02.json") {
				iw::Transform* t = SpawnLadder(iw::vector3(-1.3f, 0, -9.8f), iw::quaternion::from_euler_angles(0.3f, -0.2f, 0), &saveState->Top08LadderDown, false, "levels/canyon/top08.json");
				Space->FindEntity(t).Find<iw::Model>()->RemoveMesh(3);
			}

			break;
		}
	}

	return false;
}

iw::Transform* WorldLadderSystem::SpawnLadder(
	iw::vector3 position,
	iw::quaternion rotation,
	bool* down_saveState,
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
	l->Open = down_saveState ? *down_saveState : true;
	l->Above = above;
	l->SaveState = down_saveState;

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

		WorldLadder* wLadder = ladder.Find<WorldLadder>();

		if (wLadder->Open) {
			Bus->send<GotoLevelEvent>(wLadder->Level);
		}

		else {
			wLadder->Transition = true;
		}
	});

	Physics->AddCollisionObject(o);

	if (above) {
		if (l->Open) {
			m->GetTransform(0).Position.y = -10.0f;
			m->GetTransform(1).Position.y = -5.0f;
			m->GetTransform(2).Position.y = 0.0f;
			m->GetTransform(3).Position.y = 5.0f;
		}

		else {
			m->GetTransform(0).Position.y = 3.5f;
			m->GetTransform(1).Position.y = 4.0f;
			m->GetTransform(2).Position.y = 4.5f;
			m->GetTransform(3).Position.y = 5.0f;

			m->GetTransform(1).Position.z = -0.1f;
			m->GetTransform(2).Position.z = -0.2f;
			m->GetTransform(3).Position.z = -0.3f;
		}
	}

	else {
		if (!l->Open) {
			m->GetTransform(0).Position.y = 10.0f;
		}
	}

	return t;
}
