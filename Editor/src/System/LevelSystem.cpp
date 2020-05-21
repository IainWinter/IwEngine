#include "Systems/LevelSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Player.h"
#include "Components/DontDeleteBullets.h"

#include "iw/audio/AudioSpaceStudio.h"
#include "iw/reflection/serialization/JsonSerializer.h"
#include "iw/graphics/Model.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/engine/Time.h"

// should be generated
#include "iw/reflection/reflect/std/string.h"
#include "iw/reflection/reflect/std/vector.h"
#include "iw/reflect/math/vector2.h"
#include "iw/reflect/math/vector3.h"
#include "iw/reflect/common/Components/Transform.h"
#include "iw/reflect/Components/Bullet.h"
#include "iw/reflect/Components/Enemy.h"
#include "iw/reflect/Components/Player.h"
#include "iw/reflect/Components/LevelDoor.h"
#include "iw/reflect/Physics/Plane.h"
#include "iw/reflect/Physics/Collision/PlaneCollider.h"
#include "iw/reflect/Physics/Collision/SphereCollider.h"
#include "iw/reflect/Physics/Collision/CapsuleCollider.h"
#include "iw/reflect/Components/Level.h"
#include "iw/reflect/Components/ModelPrefab.h"

#include "iw/engine/Events/Seq/MoveToTarget.h"

LevelSystem::LevelSystem(
	iw::Entity& player)
	: iw::System<iw::Transform, iw::Model, LevelDoor>("Level")
	, playerEntity(player)
	, sequence(nullptr)
{
	//levels = {
	//	"assets/levels/forest/level1.json",
	//	"assets/levels/forest/level2.json",
	//	"assets/levels/forest/level3.json",
	//	"assets/levels/forest/level4.json",
	//	"assets/levels/forest/level5.json",
	//};

	//currentLevel = 0;

	currentLevelName = "levels/test.json";

	openColor   = iw::Color::From255(66, 201, 66, 63);
	closedColor = iw::Color::From255(201, 66, 66, 63);

	transition = false;

	//Level level;
	//level.Enemies.push_back(Enemy{ EnemyType::SPIN, Bullet { LINE, 5 }, iw::Pi2 / 24.0f, 0.12f });
	//level.Positions.push_back(0);
	//level.StageName = "";
	//level.Door = LevelDoor {
	//	LevelDoorState::LOCKED,
	//	"next.json"
	//};

	//level.Planes.push_back(iw::PlaneCollider( iw::vector3::unit_x, 16));
	//level.Planes.push_back(iw::PlaneCollider(-iw::vector3::unit_x, 16));
	//level.Planes.push_back(iw::PlaneCollider(-iw::vector3::unit_z,  9));
	//level.Planes.push_back(iw::PlaneCollider( iw::vector3::unit_z,  9));

	//iw::JsonSerializer("assets/levels/working.json").Write(level);
}

int LevelSystem::Initialize() {
	Bus->push<ResetLevelEvent>();
	//Bus->push<GoToNextLevelEvent>(currentLevelName, currentLevel.CameraFollow, currentLevel.InPosition, 0);

	return 0;
}

void LevelSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, model, door] = entity.Components.Tie<Components>();

		transform->Rotation *= iw::quaternion::from_euler_angles(iw::Time::DeltaTime() * .1f);

		if (door->ColorTimer > 0) {
			door->ColorTimer -= iw::Time::DeltaTime();
			model->GetMesh(0).Material()->Set("baseColor", iw::lerp<iw::vector4>(closedColor, openColor, 4 * (0.25f - door->ColorTimer)));
		}
	}

	if (sequence) {
		sequence->update();
	}

	//if (transition) {
	//	iw::Transform* current = levelEntity    .Find<iw::Transform>();
	//	iw::Transform* next    = nextLevelEntity.Find<iw::Transform>();

	//	iw::vector2 delta = currentLevel.LevelPosition * iw::Time::DeltaTime() * 0.5f;
	//	
	//	current->Position.x -= delta.x;
	//	current->Position.z -= delta.y;

	//	next->Position.x -= delta.x;
	//	next->Position.z -= delta.y;

	//	if (   delta.x > 0 ? next->Position.x <= 0 : next->Position.x >= 0
	//		&& delta.y > 0 ? next->Position.z <= 0 : next->Position.z >= 0)
	//	{
	//		next->Position.x = 0;
	//		next->Position.z = 0;
	//		transition = false;

	//		iw::vector3 position;
	//		position.x = currentLevel.InPosition.x;
	//		position.z = currentLevel.InPosition.y;

	//		Bus->push<StartNextLevelEvent>(currentLevel.CameraFollow);
	//		Bus->push<StartLevelEvent>(position);
	//	}
	//}

	//levelEntity.Find<iw::Transform>()->Rotation *= iw::quaternion::from_euler_angles(0, iw::Time::DeltaTime() * 0.1f, 0);
}

bool LevelSystem::On(
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

	if (   a.Index() == iw::EntityHandle::Empty.Index
		|| b.Index() == iw::EntityHandle::Empty.Index)
	{
		return false;
	}

	iw::Entity doorEnt;
	if (   a.Has<Player>()
		&& b.Has<LevelDoor>())
	{
		doorEnt = b;
	}

	else if (b.Has<Player>()
		&&   a.Has<LevelDoor>())
	{
		doorEnt = a;
	}

	if (doorEnt.Index() != iw::EntityHandle::Empty.Index) {
		LevelDoor*     door = doorEnt.Find<LevelDoor>();
		iw::Transform* tran = doorEnt.Find<iw::Transform>();
		if (door->State == LevelDoorState::OPEN) {
			door->State = LevelDoorState::LOCKED; // stops events from being spammed
			Bus->push<LoadNextLevelEvent>(door->NextLevel, iw::vector2(tran->Position.x, tran->Position.z), door->GoBack);
		}
	}

	return false;
}

bool LevelSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::RESET_LEVEL): {
			if (levelEntity != iw::EntityHandle::Empty) {
				DestroyAll(levelEntity.Find<iw::Transform>());
			}

			levelEntity = LoadLevel(currentLevelName);

			Bus->push<StartLevelEvent>(currentLevel.CameraFollow, currentLevel.InPosition);

			break;
		}
		case iw::val(Actions::UNLOCK_LEVEL_DOOR): {
			if (!levelDoor) break;

			LevelDoor* door = levelDoor.Find<LevelDoor>();
			door->State = LevelDoorState::OPEN;
			door->ColorTimer = 0.25f;

			break;
		}
		case iw::val(Actions::LOAD_NEXT_LEVEL): {
			LoadNextLevelEvent& event = e.as<LoadNextLevelEvent>();

			iw::vector2 lvpos = -currentLevel.LevelPosition;

			currentLevelName = event.LevelName.length() > 0 ? event.LevelName : levelDoor.Find<LevelDoor>()->NextLevel;
			nextLevelEntity  = LoadLevel(currentLevelName); // changes current level

			if (event.GoBack) {
				currentLevel.InPosition = -event.Position + event.Position.normalized() * 6;
			}

			else {
				lvpos = currentLevel.LevelPosition;
			}

			iw::Transform* transform = nextLevelEntity.Find<iw::Transform>();
			transform->Position.x = lvpos.x;
			transform->Position.z = lvpos.y;

			Bus->push<GoToNextLevelEvent>(currentLevelName, currentLevel.CameraFollow, currentLevel.InPosition, lvpos);

			break;
		}
		case iw::val(Actions::AT_NEXT_LEVEL): {
			if (nextLevelEntity != iw::EntityHandle::Empty) {
				DestroyAll(levelEntity.Find<iw::Transform>());

				levelEntity = nextLevelEntity;
				nextLevelEntity = iw::Entity();

				iw::Transform* current = levelEntity.Find<iw::Transform>();
				current->Position = 0;
			}

			Bus->push<StartLevelEvent>(currentLevel.CameraFollow, currentLevel.InPosition);

			if (sequence) {
				sequence->restart();
			}

			break;
		}
	}

	return false;
}

iw::Entity LevelSystem::LoadLevel(
	std::string name)
{
	//Space->Clear();

	iw::JsonSerializer("assets/" + name).Read(currentLevel);

	// Stage

	//float scale = 2.0f;
	//float scaleOutX = 1.6f;
	//float scaleOutY = 1.8f;
	//
	//int x = 16 * scaleOutX;
	//int y = 9 * scaleOutY;
	//
	//for (int l = 0; l < 1; l++) {
	//	for (int i = -x; i < x; i += 4) {
	//		LoadTree("models/forest/tree.dae", iw::Transform {
	//			iw::vector3(i, 0, y),
	//			scale,
	//			iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
	//				* iw::quaternion::from_axis_angle(iw::vector3::unit_y, rand() / (float)RAND_MAX * iw::Pi2)
	//		});
	//	}
	//
	//	for (int i = -x; i < x; i += 4) {
	//		LoadTree("models/forest/tree.dae", iw::Transform {
	//			iw::vector3(i, 0, -y),
	//			scale,
	//			iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
	//				* iw::quaternion::from_axis_angle(iw::vector3::unit_y, rand() / (float)RAND_MAX * iw::Pi2)
	//		});
	//	}
	//
	//	for (int i = -y; i <= y; i += 3) {
	//		LoadTree("models/forest/tree.dae", iw::Transform {
	//			iw::vector3(x, 0, i),
	//			scale,
	//			iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
	//				* iw::quaternion::from_axis_angle(iw::vector3::unit_y, rand() / (float)RAND_MAX * iw::Pi2)
	//		});
	//	}
	//
	//	for (int i = -y + 3; i < y; i += 3) {
	//		LoadTree("models/forest/tree.dae", iw::Transform {
	//			iw::vector3(-x, 0, i),
	//			scale,
	//			iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2)
	//				* iw::quaternion::from_axis_angle(iw::vector3::unit_y, rand() / (float)RAND_MAX * iw::Pi2)
	//		});
	//	}
	//
	//	scaleOutX = 1.6f;
	//	scaleOutY = 1.8f;
	//
	//	x = 16 * scaleOutX;
	//	y = 9 * scaleOutY;
	//}

	iw::Transform* levelTransform = nullptr;
	iw::Entity level;

	for (ModelPrefab& prefab : currentLevel.Models) {
		iw::ref<iw::Model> model = Asset->Load<iw::Model>(prefab.ModelName);

		for (iw::Mesh& mesh : model->GetMeshes()) {
			if (mesh.Data()->IsInitialized()) {
				continue;
			}

			mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/vct/vct.shader"));
			mesh.Material()->SetTexture("shadowMap", Asset->Load<iw::Texture>("SunShadowMap"));   // shouldnt be part of material
			mesh.Material()->SetTexture("shadowMap2", Asset->Load<iw::Texture>("LightShadowMap")); // shouldnt be part of material

			mesh.Material()->Set("roughness", 0.9f);
			mesh.Material()->Set("metallic", 0.1f);
			mesh.Material()->Set("reflectance", 0.1f);

			mesh.Material()->Initialize(Renderer->Device);

			//floor->Meshes[i].SetIsStatic(true);
			//mesh.Data()->GenTangents();
			mesh.Data()->Initialize(Renderer->Device);
		}

		iw::Entity entity = Space->CreateEntity<iw::Transform, iw::Model>();
		iw::Transform* t = entity.Set<iw::Transform>(prefab.Transform);
		                   entity.Set<iw::Model>(*model);

		if(levelTransform) {
			levelTransform->AddChild(t);
		}

		else {
			levelTransform = t;
			level = entity;
		}
	}

	// These should all be spawn x events
	// Not really sure what the best way to sort loading levels is. Im sure it will become apparent in time

	// Enemies

	firstEnemy = iw::Entity();

	for (size_t i = 0; i < currentLevel.Enemies.size(); i++) {
		iw::Entity ent = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::CollisionObject, Enemy>();
		
		Enemy*               e = ent.Set<Enemy>(currentLevel.Enemies[i]);
		iw::Transform*       t = ent.Set<iw::Transform>();
		iw::SphereCollider*  s = ent.Set<iw::SphereCollider>(iw::vector3::zero, 1.0f);
		iw::CollisionObject* c = ent.Set<iw::CollisionObject>();

		if (!firstEnemy) {
			firstEnemy = ent;
		}

		switch (currentLevel.Enemies[i].Type) {
			case EnemyType::MINI_BOSS_BOX_SPIN: {
				e->Health = 3;
				e->ScoreMultiple = 10;
				t->Scale = 0.75f;
				ent.Set<iw::Model>(*Asset->Load<iw::Model>("Box"));

				break;
			}
			default: {
				e->Health = 1;
				e->ScoreMultiple = 1;
				ent.Set<iw::Model>(*Asset->Load<iw::Model>("Tetrahedron"));
				break;
			}
		}

		//e->Timer = e->ChargeTime;

		t->Position.x = currentLevel.Positions[i].x;
		t->Position.z = currentLevel.Positions[i].y;
		t->Position.y = 1;

		levelTransform->AddChild(t);

		c->SetCol(s);
		c->SetTrans(t);
		//c->SetIsTrigger(true); // temp should make collision layers

		c->SetOnCollision([&](iw::Manifold& man, float dt) {
			iw::Entity enemy  = Space->FindEntity<iw::CollisionObject>(man.ObjA);
			iw::Entity player = Space->FindEntity<iw::Rigidbody>(man.ObjB);

			if (!enemy || !player) {
				return;
			}

			Enemy*  enemyComponent  = enemy .Find<Enemy>();
			Player* playerComponent = player.Find<Player>();

			if (!enemyComponent || !playerComponent) {
				return;
			}

			if (playerComponent->Timer <= 0.0f) {
				return;
			}

			enemyComponent->Health -= 1;

			if (enemyComponent->Health > 0) {
				return;
			}

			iw::Transform* transform  = enemy.Find<iw::Transform>();

			float score = floor(5 * (1.0f - playerComponent->Timer / playerComponent->DashTime)) * 200 + 200;

			Audio->AsStudio()->CreateInstance("enemyDeath");

			Bus->push<SpawnEnemyDeath>(transform->Position, transform->Parent());
			Bus->push<GiveScoreEvent>(transform->Position, score * enemyComponent->ScoreMultiple);

			transform->SetParent(nullptr);
			Space->DestroyEntity(enemy.Index());
		});

		Physics->AddCollisionObject(c);
	}

	// Colliders

	for (iw::PlaneCollider& prefab : currentLevel.Planes) {
		iw::Entity ent = Space->CreateEntity<iw::Transform, iw::PlaneCollider, iw::CollisionObject>();
		
		iw::Transform*       transform = ent.Set<iw::Transform>();
		iw::PlaneCollider*   collider  = ent.Set<iw::PlaneCollider>(prefab);
		iw::CollisionObject* object    = ent.Set<iw::CollisionObject>();

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);

		Physics->AddCollisionObject(object);
	}

	for (iw::CapsuleCollider& prefab : currentLevel.Capsules) {
		iw::Entity ent;
		
		if (   currentLevelName == "levels/forest/forest15.json"
			|| currentLevelName == "levels/forest/forest16.json"
			|| currentLevelName == "levels/forest/forest17.json")
		{
			ent = Space->CreateEntity<iw::Transform, iw::CapsuleCollider, iw::CollisionObject, DontDeleteBullets>();
		}

		else {
			ent = Space->CreateEntity<iw::Transform, iw::CapsuleCollider, iw::CollisionObject>();
		}


		iw::Transform*       transform = ent.Set<iw::Transform>(iw::vector3::unit_y);
		iw::CapsuleCollider* collider  = ent.Set<iw::CapsuleCollider>(prefab);
		iw::CollisionObject* object    = ent.Set<iw::CollisionObject>();

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);

		Physics->AddCollisionObject(object);
	}

	for (iw::SphereCollider& prefab : currentLevel.Spheres) {
		iw::Entity ent = Space->CreateEntity<iw::Transform, iw::SphereCollider, iw::CollisionObject>();

		iw::Transform*       transform = ent.Set<iw::Transform>(iw::vector3::unit_y);
		iw::SphereCollider*  collider  = ent.Set<iw::SphereCollider>(prefab);
		iw::CollisionObject* object    = ent.Set<iw::CollisionObject>();

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);

		Physics->AddCollisionObject(object);
	}

	// Doors

	for (size_t i = 0; i < currentLevel.Doors.size(); i++) {
		iw::Entity ent = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::CollisionObject, LevelDoor>();
	
		if (i == 0) {
			levelDoor = ent;
		}

		LevelDoor*           door      = ent.Set<LevelDoor>(currentLevel.Doors[i]);
		iw::Model*           model     = ent.Set<iw::Model>(*Asset->Load<iw::Model>("Door"));
		iw::Transform*       transform = ent.Set<iw::Transform>(iw::vector3(currentLevel.DoorPositions[i].x, 1, currentLevel.DoorPositions[i].y), 5.0f);
		iw::SphereCollider*  collider  = ent.Set<iw::SphereCollider>(iw::vector3::zero, 1.0f);
		iw::CollisionObject* object    = ent.Set<iw::CollisionObject>();

		iw::ref<iw::Material> material = model->GetMesh(0).Material()->MakeInstance();

		material->SetCastShadows(false);

		if (door->State == LevelDoorState::OPEN) {
			material->Set("baseColor", openColor);
		}

		else {
			material->Set("baseColor", closedColor);
		}

		model->GetMesh(0).SetMaterial(material);

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);
		object->SetIsTrigger(true);

		Physics->AddCollisionObject(object);
	}

	// Spawning items
	
	if (currentLevelName == "levels/forest/forest05.a.json") {
		Bus->push<SpawnItemEvent>(Item{ NOTE, 0 },       iw::vector3(0, 1, 5), levelTransform);
		Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 0 }, iw::vector3(0, 1, 8), levelTransform);
	}

	// run a cut scene

	// Lucas - edit this for now \/
	// You can edit line 48 to make it start on level 2
	// to destory the enemy, just use the firstEnemy variable from line 346
	// if it keeps crashing just make it move underground and I'll figure it out later (i.e. set 'y' to like -5)

	if (currentLevelName == "levels/forest/forest02.json") {
		Space->DestroyEntity(otherGuy.Index());
		otherGuy = Space->CreateEntity<iw::Transform, iw::Mesh>();

		otherGuy.Set<iw::Transform>(
			iw::vector3(-5, 1, -16 /*starting location */ /*z axis is reversed xd (- is going twoards the top of the screen)*/),
			0.75f /*scale*/
		);

		otherGuy.Set<iw::Mesh>(Asset->Load<iw::Model>("Sphere")->GetMesh(0).MakeInstance()); // "Sphere" gets created at line 246 of SandboxLayer.cpp

		delete sequence;
		sequence = new iw::event_seq();
		sequence->add(new iw::MoveToTarget(otherGuy, iw::vector3(-5, 1, -5))); // sample of what to kinda do
		sequence->add(new iw::MoveToTarget(otherGuy, iw::vector3(-4, 1,  5))); // cutscene is going to be 
		sequence->add(new iw::MoveToTarget(otherGuy, iw::vector3(-3, 1, -3)));
		sequence->add(new iw::MoveToTarget(otherGuy, iw::vector3(-12, 1, 0)));
		sequence->add(new iw::MoveToTarget(firstEnemy, iw::vector3(-10, 1, 0)));
	}

	return level;
}

void LevelSystem::LoadTree(
	std::string name,
	iw::Transform transform)
{
	//iw::ref<iw::Model> tree = Asset->Load<iw::Model>(name);

	//if (tree->Meshes[0].VertexArray == nullptr) {
	//	for (size_t i = 0; i < tree->MeshCount; i++) {
	//		iw::ref<iw::Material>& mat = tree->Meshes[i].Material;

	//		mat->SetShader(Asset->Load<iw::Shader>("shaders/pbr.shader"));
	//		mat->SetTexture("shadowMap",  Asset->Load<iw::Texture>("SunShadowMap"));   // shouldnt be part of material
	//		mat->SetTexture("shadowMap2", Asset->Load<iw::Texture>("LightShadowMap")); // shouldnt be part of material
	//		mat->Initialize(Renderer->Device);

	//		mat->Set("roughness", 0.7f);
	//		mat->Set("metallic", 0.0f);

	//		tree->Meshes[i].GenTangents();
	//		tree->Meshes[i].SetIsStatic(true);
	//		tree->Meshes[i].Initialize(Renderer->Device);
	//	}
	//}

	//iw::ref<iw::Texture> leavesAlpha = Asset->Load<iw::Texture>("textures/forest/tree/leaves/alpha.jpg");
	//leavesAlpha->Initialize(Renderer->Device);

	//tree->Meshes[1].Material->SetTexture("alphaMaskMap", leavesAlpha);

	//iw::Entity ent = Space->CreateEntity<iw::Transform, iw::Model>();
	//ent.Set<iw::Transform>(transform);
	//ent.Set<iw::Model>(*tree);
}

iw::Entity LevelSystem::LoadFloor(
	std::string name,
	iw::Transform transform)
{
	iw::ref<iw::Model> floor = Asset->Load<iw::Model>(name);

	for (iw::Mesh& mesh : floor->GetMeshes()) {
		if (mesh.Data()->IsInitialized()) {
			continue;
		}

		mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/vct/vct.shader"));
		mesh.Material()->SetTexture("shadowMap",  Asset->Load<iw::Texture>("SunShadowMap"));   // shouldnt be part of material
		mesh.Material()->SetTexture("shadowMap2", Asset->Load<iw::Texture>("LightShadowMap")); // shouldnt be part of material

		mesh.Material()->Set("roughness", 0.9f);
		mesh.Material()->Set("metallic", 0.1f);
		mesh.Material()->Set("reflectance", 0.1f);

		mesh.Material()->Initialize(Renderer->Device);

		//floor->Meshes[i].SetIsStatic(true);
		//mesh.Data()->GenTangents();
		mesh.Data()->Initialize(Renderer->Device);
	}

	iw::Entity entity = Space->CreateEntity<iw::Transform, iw::Model>();
	entity.Set<iw::Transform>(transform);
	entity.Set<iw::Model>(*floor);

	return entity;
}

void LevelSystem::DestroyAll(
	iw::Transform* transform) // should be in engine somewhere
{
	if (transform) {
		for (iw::Transform* child : transform->Children()) {
			DestroyAll(child);
		}

		Space->DestroyEntity(Space->FindEntity(transform).Index());
	}
}
