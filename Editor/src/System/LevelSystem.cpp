#include "Systems/LevelSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Player.h"
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
#include "iw/reflect/Components/Bullet.h"
#include "iw/reflect/Components/Enemy.h"
#include "iw/reflect/Components/Player.h"
#include "iw/reflect/Components/LevelDoor.h"
#include "iw/reflect/Physics/Plane.h"
#include "iw/reflect/Physics/Collision/PlaneCollider.h"
#include "iw/reflect/Physics/Collision/SphereCollider.h"
#include "iw/reflect/Physics/Collision/CapsuleCollider.h"
#include "iw/reflect/Components/Level.h"
#include <Components\DontDeleteBullets.h>

LevelSystem::LevelSystem(
	iw::Entity& player)
	: iw::System<iw::Transform, iw::Model, LevelDoor>("Level")
	, playerEntity(player)
{
	//levels = {
	//	"assets/levels/forest/level1.json",
	//	"assets/levels/forest/level2.json",
	//	"assets/levels/forest/level3.json",
	//	"assets/levels/forest/level4.json",
	//	"assets/levels/forest/level5.json",
	//};

	//currentLevel = 0;

	currentLevelName = "levels/forest/forest1.json";

	openColor   = iw::Color::From255(66, 201, 66, 127);
	closedColor = iw::Color::From255(201, 66, 66, 127);

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
			model->Meshes[0].Material->Set("albedo", iw::lerp<iw::vector4>(closedColor, openColor, 4 * (0.25f - door->ColorTimer)));
		}
	}

	//if (transition) {
	//	iw::Transform* current = levelEntity    .FindComponent<iw::Transform>();
	//	iw::Transform* next    = nextLevelEntity.FindComponent<iw::Transform>();

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

	//levelEntity.FindComponent<iw::Transform>()->Rotation *= iw::quaternion::from_euler_angles(0, iw::Time::DeltaTime() * 0.1f, 0);
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
	if (   a.HasComponent<Player>()
		&& b.HasComponent<LevelDoor>())
	{
		doorEnt = b;
	}

	else if (b.HasComponent<Player>()
		&&   a.HasComponent<LevelDoor>())
	{
		doorEnt = a;
	}

	if (doorEnt.Index() != iw::EntityHandle::Empty.Index) {
		LevelDoor* door = doorEnt.FindComponent<LevelDoor>();
		if (door->State == LevelDoorState::OPEN) {
			Bus->push<LoadNextLevelEvent>();
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
				DestroyAll(levelEntity.FindComponent<iw::Transform>());
			}

			levelEntity = LoadLevel(currentLevelName);

			Bus->push<StartLevelEvent>(currentLevel.CameraFollow, currentLevel.InPosition);

			break;
		}
		case iw::val(Actions::UNLOCK_LEVEL_DOOR): {
			LevelDoor* doorComp = currentDoor.FindComponent<LevelDoor>();
			doorComp->State = LevelDoorState::OPEN;
			doorComp->ColorTimer = 0.25f;

			break;
		}
		case iw::val(Actions::LOAD_NEXT_LEVEL): {
			currentDoor.FindComponent<LevelDoor>()->State = LevelDoorState::LOCKED; // stops events from being spammed
			
			//transition       = true;
			currentLevelName = currentLevel.Door.NextLevel;
			nextLevelEntity  = LoadLevel(currentLevelName);

			iw::Transform* transform = nextLevelEntity.FindComponent<iw::Transform>();
			transform->Position.x = currentLevel.LevelPosition.x;
			transform->Position.z = currentLevel.LevelPosition.y;

			Bus->push<GoToNextLevelEvent>(currentLevel.StageName, currentLevel.CameraFollow, currentLevel.InPosition, currentLevel.LevelPosition);

			break;
		}
		case iw::val(Actions::AT_NEXT_LEVEL): {
			if (nextLevelEntity != iw::EntityHandle::Empty) {
				DestroyAll(levelEntity.FindComponent<iw::Transform>());

				levelEntity = nextLevelEntity;
				nextLevelEntity = iw::Entity();

				iw::Transform* current = levelEntity.FindComponent<iw::Transform>();
				current->Position = 0;
			}

			Bus->push<StartLevelEvent>(currentLevel.CameraFollow, currentLevel.InPosition);

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

	iw::Entity level = LoadFloor(currentLevel.StageName, iw::Transform {
		iw::vector3(0, 0, 0),
		iw::vector3(1),
		iw::quaternion::identity
	});

	iw::Transform* levelTransform = level.FindComponent<iw::Transform>();

	// Enemies

	for (size_t i = 0; i < currentLevel.Enemies.size(); i++) {
		iw::Entity enemy = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::CollisionObject, Enemy>();
		
		                         enemy.SetComponent<iw::Model>(*Asset->Load<iw::Model>("Tetrahedron"));
		Enemy*               e = enemy.SetComponent<Enemy>(currentLevel.Enemies[i]);
		iw::Transform*       t = enemy.SetComponent<iw::Transform>();
		iw::SphereCollider*  s = enemy.SetComponent<iw::SphereCollider>(iw::vector3::zero, 1.0f);
		iw::CollisionObject* r = enemy.SetComponent<iw::CollisionObject>();

		//e->Timer = e->ChargeTime;

		t->Position.x = currentLevel.Positions[i].x;
		t->Position.z = currentLevel.Positions[i].y;
		t->Position.y = 1;

		levelTransform->AddChild(t);

		r->SetCol(s);
		r->SetTrans(t);

		Physics->AddCollisionObject(r);
	}

	// Colliders

	for (iw::PlaneCollider& prefab : currentLevel.Planes) {
		iw::Entity enemy = Space->CreateEntity<iw::Transform, iw::PlaneCollider, iw::CollisionObject>();
		
		iw::Transform*       transform = enemy.SetComponent<iw::Transform>();
		iw::PlaneCollider*   collider  = enemy.SetComponent<iw::PlaneCollider>(prefab);
		iw::CollisionObject* object    = enemy.SetComponent<iw::CollisionObject>();

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);

		Physics->AddCollisionObject(object);
	}

	for (iw::CapsuleCollider& prefab : currentLevel.Capsules) {
		iw::Entity enemy;
		
		if (   currentLevel.StageName == "models/block/forest15.dae"
			|| currentLevel.StageName == "models/block/forest16.dae"
			|| currentLevel.StageName == "models/block/forest17.dae")
		{
			enemy = Space->CreateEntity<iw::Transform, iw::CapsuleCollider, iw::CollisionObject, DontDeleteBullets>();
		}

		else {
			enemy = Space->CreateEntity<iw::Transform, iw::CapsuleCollider, iw::CollisionObject>();
		}


		iw::Transform*       transform = enemy.SetComponent<iw::Transform>(iw::vector3::unit_y);
		iw::CapsuleCollider* collider  = enemy.SetComponent<iw::CapsuleCollider>(prefab);
		iw::CollisionObject* object    = enemy.SetComponent<iw::CollisionObject>();

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);

		Physics->AddCollisionObject(object);
	}

	for (iw::SphereCollider& prefab : currentLevel.Spheres) {
		iw::Entity enemy = Space->CreateEntity<iw::Transform, iw::SphereCollider, iw::CollisionObject>();

		iw::Transform*       transform = enemy.SetComponent<iw::Transform>(iw::vector3::unit_y);
		iw::SphereCollider*  collider  = enemy.SetComponent<iw::SphereCollider>(prefab);
		iw::CollisionObject* object    = enemy.SetComponent<iw::CollisionObject>();

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);

		Physics->AddCollisionObject(object);
	}

	// Player

	//iw::vector3 playerPos = currentLevel.Door.NextLevelPosition;
	//playerPos.z = playerPos.y;
	//playerPos.y = 1.0f;
	//
	//iw::Entity player = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Player>();
	//
	//                         player.SetComponent<iw::Model>(*Asset->Load<iw::Model>("Player"));
	//                         player.SetComponent<Player>(currentLevel.Player);
	//iw::Transform*      tp = player.SetComponent<iw::Transform>(playerPos, iw::vector3(.75f));
	//iw::SphereCollider* sp = player.SetComponent<iw::SphereCollider>(iw::vector3::zero, .75f);
	//iw::Rigidbody*      rp = player.SetComponent<iw::Rigidbody>();
	//
	//levelTransform->AddChild(tp);
	//
	//rp->SetMass(1);
	//rp->SetCol(sp);
	//rp->SetTrans(tp);
	//rp->SetStaticFriction(.1f);
	//rp->SetDynamicFriction(.02f);
	//
	//rp->SetIsLocked(iw::vector3(0, 1, 0));
	//rp->SetLock    (iw::vector3(0, 1, 0));
	//
	//Physics->AddRigidbody(rp);

	// Door

	currentDoor = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::CollisionObject, LevelDoor>();
	
	LevelDoor*           door      = currentDoor.SetComponent<LevelDoor>(currentLevel.Door);
	iw::Model*           model     = currentDoor.SetComponent<iw::Model>(*Asset->Load<iw::Model>("Door"));
	iw::Transform*       transform = currentDoor.SetComponent<iw::Transform>(iw::vector3(currentLevel.OutPosition.x, 1, currentLevel.OutPosition.y), 5.0f);
	iw::SphereCollider*  collider  = currentDoor.SetComponent<iw::SphereCollider>(iw::vector3::zero, 1.0f);
	iw::CollisionObject* object    = currentDoor.SetComponent<iw::CollisionObject>();

	if (door->State == LevelDoorState::OPEN) {
		door->ColorTimer = 0.25f;
	}

	model->Meshes[0].Material->Set("albedo", closedColor);
	model->Meshes[0].Material->SetCastShadows(false);

	levelTransform->AddChild(transform);

	object->SetCol(collider);
	object->SetTrans(transform);
	object->SetIsTrigger(true);

	Physics->AddCollisionObject(object);

	// Camera recreated in layer from \/

	return level;
}

void LevelSystem::LoadTree(
	std::string name,
	iw::Transform transform)
{
	iw::ref<iw::Model> tree = Asset->Load<iw::Model>(name);

	if (tree->Meshes[0].VertexArray == nullptr) {
		for (size_t i = 0; i < tree->MeshCount; i++) {
			iw::ref<iw::Material>& mat = tree->Meshes[i].Material;

			mat->SetShader(Asset->Load<iw::Shader>("shaders/pbr.shader"));
			mat->SetTexture("shadowMap",  Asset->Load<iw::Texture>("SunShadowMap"));   // shouldnt be part of material
			mat->SetTexture("shadowMap2", Asset->Load<iw::Texture>("LightShadowMap")); // shouldnt be part of material
			mat->Initialize(Renderer->Device);

			mat->Set("roughness", 0.7f);
			mat->Set("metallic", 0.0f);

			tree->Meshes[i].GenTangents();
			tree->Meshes[i].SetIsStatic(true);
			tree->Meshes[i].Initialize(Renderer->Device);
		}
	}

	iw::ref<iw::Texture> leavesAlpha = Asset->Load<iw::Texture>("textures/forest/tree/leaves/alpha.jpg");
	leavesAlpha->Initialize(Renderer->Device);

	tree->Meshes[1].Material->SetTexture("alphaMaskMap", leavesAlpha);

	iw::Entity ent = Space->CreateEntity<iw::Transform, iw::Model>();
	ent.SetComponent<iw::Transform>(transform);
	ent.SetComponent<iw::Model>(*tree);
}

iw::Entity LevelSystem::LoadFloor(
	std::string name,
	iw::Transform transform)
{
	iw::ref<iw::Model> floor = Asset->Load<iw::Model>(name);

	if (floor->Meshes[0].VertexArray == nullptr) {
		for (size_t i = 0; i < floor->MeshCount; i++) {
			iw::ref<iw::Material>& mat = floor->Meshes[i].Material;

			mat->SetShader(Asset->Load<iw::Shader>("shaders/pbr.shader"));
			mat->SetTexture("shadowMap",  Asset->Load<iw::Texture>("SunShadowMap"));   // shouldnt be part of material
			mat->SetTexture("shadowMap2", Asset->Load<iw::Texture>("LightShadowMap")); // shouldnt be part of material
			mat->Initialize(Renderer->Device);

			mat->Set("roughness", 0.9f);
			mat->Set("metallic", 0.1f);

			floor->Meshes[i].GenTangents();
			//floor->Meshes[i].SetIsStatic(true);
			floor->Meshes[i].Initialize(Renderer->Device);
		}
	}

	iw::Entity entity = Space->CreateEntity<iw::Transform, iw::Model>();
	entity.SetComponent<iw::Transform>(transform);
	entity.SetComponent<iw::Model>(*floor);

	return entity;
}

void LevelSystem::DestroyAll(
	iw::Transform* transform)
{
	if (transform) {
		for (iw::Transform* child : transform->Children()) {
			DestroyAll(child);
		}

		Space->DestroyEntity(Space->FindEntity(transform).Index());
	}
}
