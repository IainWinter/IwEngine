#include "Systems/WorldLadderSystem.h"

#include "Components/WorldLadder.h"
#include "Components/Player.h"
#include "Events/ActionEvents.h"

#include "iw/common/Components/Transform.h"
#include "iw/graphics/Model.h"

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
				SpawnLadder(0, iw::quaternion::identity, saveState->Cave03LadderDown, "levels/canyon/canyon04.a.json");
			}

			else if (event.LevelName == "levels/canyon/cave06.json") {
				SpawnLadder(iw::vector3(3.5f, 0, -14), iw::quaternion::from_euler_angles(0, 0.7f, 0), saveState->Cave06LadderDown, "levels/canyon/canyon07.a.json");
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
	std::string level)
{
	iw::Entity ladder = Space->Instantiate(ladderPrefab);

	iw::Transform* t = ladder.Find<iw::Transform>();
	iw::Model*     m = ladder.Find<iw::Model>();
	WorldLadder*   l = ladder.Find<WorldLadder>();

	if (!down) {
		position.y = 10.0f;
	}

	t->Position = position;
	t->Rotation = rotation;

	l->Level = level;
	l->Open = down;

	t->SetParent(currentLevel.Find<iw::Transform>());
}
