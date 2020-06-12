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
#include "iw/engine/Events/Seq/FadeValue.h"
#include "iw/engine/Events/Seq/Delay.h"
#include "iw/engine/Events/Seq/PlaySound.h"
#include "iw/engine/Events/Seq/ShowText.h"
#include "iw/engine/Events/Seq/DestroyEntity.h"
#include "iw/engine/Components/UiElement_temp.h"

struct OtherGuyTag {
	int Health = 3;
};

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

	currentLevelName = "levels/forest/forest01.json";

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

float speed = 1.0f; // garbo
float timeout = 0.0f;
iw::vector2 lastLevelPosition = 0;

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

	sequence.Update();

	if (transition) {
		iw::Transform* current = levelEntity    .Find<iw::Transform>();
		iw::Transform* next    = nextLevelEntity.Find<iw::Transform>();

		speed += iw::Time::DeltaTime() * 5;

		LevelDoor* currentDoor = levelDoor.Find<LevelDoor>();
		//LevelDoor* nextDoor = nextLevelDoor.Find<LevelDoor>(); // was crashing because currentDoor was moved to nextDoor for some reason??

		iw::vector3 target = currentLevel.LevelPosition;
		if (   levelDoor != iw::EntityHandle::Empty
			&& currentDoor && currentDoor->GoBack
			/*|| nextDoor    && nextDoor->GoBack*/)
		{
			target = -lastLevelPosition;
		}

		target.z = target.y;
		target.y = 0;

		current->Position = iw::lerp(current->Position, -target,        iw::Time::DeltaTime() * speed);
		next   ->Position = iw::lerp(next   ->Position, iw::vector3(0), iw::Time::DeltaTime() * speed);

		if (   iw::Time::TotalTime() - timeout > 0
			&& iw::almost_equal(next->Position.x, 0.0f, 2)
			&& iw::almost_equal(next->Position.z, 0.0f, 2))
		{
			Bus->push<AtNextLevelEvent>();

			next->Position.x = 0.0f;
			next->Position.z = 0.0f;
			transition = false;
			speed = 2;
		}
	}
}

bool LevelSystem::On(
	iw::CollisionEvent& e)
{
	iw::Entity doorEntity, playerEntity;
	bool noent = GetEntitiesFromManifold<LevelDoor, Player>(e.Manifold, doorEntity, playerEntity);

	if (noent) {
		return false;
	}

	LevelDoor*     door = doorEntity.Find<LevelDoor>();
	iw::Transform* tran = doorEntity.Find<iw::Transform>();

	if (door->State == LevelDoorState::OPEN) {
		door->State = LevelDoorState::LOCKED; // stops events from being spammed
		Bus->push<LoadNextLevelEvent>(door->NextLevel, iw::vector2(tran->Position.x, tran->Position.z), door->GoBack);
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

			if (levelDoor == iw::EntityHandle::Empty) {
				levelDoor = nextLevelDoor;
			}

			Bus->push<StartLevelEvent>(currentLevelName, currentLevel.CameraFollow, currentLevel.InPosition);

			sequence.Restart();

			break;
		}
		case iw::val(Actions::UNLOCK_LEVEL_DOOR): {
			if (!levelDoor) break;

			LevelDoor* door = levelDoor.Find<LevelDoor>();
			if (door == nullptr) {
				door = nextLevelDoor.Find<LevelDoor>();
			}

			door->State = LevelDoorState::OPEN;
			door->ColorTimer = 0.25f;

			break;
		}
		case iw::val(Actions::LOAD_NEXT_LEVEL): {
			LoadNextLevelEvent& event = e.as<LoadNextLevelEvent>();

			iw::vector2 lvpos = -currentLevel.LevelPosition;

			if (event.LevelName.length() > 0) {
				currentLevelName = event.LevelName;
			}

			else if (nextLevelDoor != iw::EntityHandle::Empty) {
				currentLevelName = nextLevelDoor.Find<LevelDoor>()->NextLevel;
			}

			else if (levelDoor != iw::EntityHandle::Empty) {
				currentLevelName = levelDoor.Find<LevelDoor>()->NextLevel;
			}

			else {
				LOG_WARNING << ":(";
			}

			nextLevelEntity = LoadLevel(currentLevelName); // changes current level

			if (event.GoBack) {
				currentLevel.InPosition = -event.Position + event.Position.normalized() * 6;
			}

			else {
				lvpos = currentLevel.LevelPosition;
			}

			iw::Transform* transform = nextLevelEntity.Find<iw::Transform>();
			transform->Position.x = lvpos.x;
			transform->Position.z = lvpos.y;

			transition = true;

			Bus->push<GoToNextLevelEvent>(currentLevelName, currentLevel.CameraFollow, currentLevel.InPosition, lvpos);

			break;
		}
		case iw::val(Actions::AT_NEXT_LEVEL): {
			if (nextLevelEntity != iw::EntityHandle::Empty) {
				DestroyAll(levelEntity.Find<iw::Transform>());

				lastLevelPosition = currentLevel.LevelPosition;

				levelEntity = nextLevelEntity;
				nextLevelEntity = iw::Entity();

				levelDoor = nextLevelDoor;
				nextLevelDoor = iw::Entity();

				iw::Transform* current = levelEntity.Find<iw::Transform>();
				current->Position = 0;
			}

			Bus->push<StartLevelEvent>(currentLevelName, currentLevel.CameraFollow, currentLevel.InPosition);

			sequence.Restart();

			break;
		}
	}

	return false;
}

iw::Entity LevelSystem::LoadLevel(
	std::string name)
{
	iw::JsonSerializer("assets/" + name).Read(currentLevel);

	iw::Transform* levelTransform = nullptr;
	iw::Entity level;

	int iii = 0;

	for (ModelPrefab& prefab : currentLevel.Models) {
		iw::ref<iw::Model> model = Asset->Load<iw::Model>(prefab.ModelName);

		for (iw::Mesh& mesh : model->GetMeshes()) {
			mesh.SetMaterial(mesh.Material()->MakeInstance());

			if (iii == 0 && false) { // ground
				mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/phong.shader"));

				if (mesh.Data()->Description().HasBuffer(iw::bName::COLOR)) {
					mesh.Material()->SetTexture("diffuseMap2", Asset->Load<iw::Texture>("textures/dirt/baseColor.jpg"));
					mesh.Material()->SetTexture("normalMap2", Asset->Load<iw::Texture>("textures/dirt/normal.jpg"));
				}
			}

			else {
				mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/vct/vct.shader"));

				if (mesh.Data()->Description().HasBuffer(iw::bName::COLOR)) {
					mesh.Material()->SetTexture("diffuseMap2", Asset->Load<iw::Texture>("textures/dirt/baseColor.jpg"));
					mesh.Material()->SetTexture("normalMap2", Asset->Load<iw::Texture>("textures/dirt/normal.jpg"));
				}
			}

			iii++;
			
			mesh.Material()->SetTexture("shadowMap", Asset->Load<iw::Texture>("SunShadowMap"));
			//mesh.Material()->SetTexture("shadowMap2", Asset->Load<iw::Texture>("LightShadowMap")); // shouldnt be part of material

			//mesh.Material()->Set("roughness", 0.9f);
			//mesh.Material()->Set("metallic", 0.1f);
			mesh.Material()->Set("reflectance", 0.0f);
			mesh.Material()->Set("refractive", 0.0f);

			mesh.Material()->Set("indirectDiffuse", 1);
			mesh.Material()->Set("indirectSpecular", 0);

			//if (iii == 1) {
			//	mesh.Material()->Set("baseColor", iw::Color(0, 0, 1));
			//}

			//iii++;

			//floor->Meshes[i].SetIsStatic(true);
			//mesh.Data()->GenTangents()x;
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
		iw::vector3 position;
		position.x = currentLevel.Positions[i].x;
		position.z = currentLevel.Positions[i].y;
		position.y = 1;

		Bus->send<SpawnEnemyEvent>(currentLevel.Enemies[i], position, 0, levelTransform);

		if (!firstEnemy) {
			firstEnemy = Space->FindEntity(levelTransform->Children().back());
		}
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
			nextLevelDoor = ent;
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

		material->Set("emissive", 2.0f);

		material->Set("indirectDiffuse",  0);
		material->Set("indirectSpecular", 0);

		model->GetMesh(0).SetMaterial(material);

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);
		object->SetIsTrigger(true);

		Physics->AddCollisionObject(object);
	}

	// Spawning items
	
	sequence = CreateSequence();

	//seq.SetVars(Space, Renderer, Asset, Physics, Audio, Bus);

	if (currentLevelName == "levels/forest/forest05.a.json") {
		Bus->push<SpawnItemEvent>(Item{ NOTE, 0 },       iw::vector3(3, 1, -2), levelTransform);
		Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 0 }, iw::vector3(0, 1, 3),  levelTransform);
	}

	// run a cut scene

	if (currentLevelName == "levels/forest/forest01.json") {
		iw::Mesh t = Asset->Load<iw::Font>("fonts/Arial.fnt")->GenerateMesh("A lone soldier longs for his king...", .005f, 1);
		t.SetMaterial(Asset->Load<iw::Material>("materials/Font")->MakeInstance());
		t.Material()->Set("color", iw::Color(1, 1, 1, 0));

		iw::ref<iw::Shader> shader = Asset->Load<iw::Shader>("shaders/simple.shader");
		Renderer->InitShader(shader, iw::CAMERA);

		float alpha = 1;

		iw::Material material(shader);
		material.Set("color", iw::Color(0, 0, 0, alpha));

		iw::Mesh bg = Renderer->ScreenQuad().MakeInstance();
		bg.SetMaterial(REF<iw::Material>(material));

		iw::Entity text = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();
		iw::Entity back = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();

		text.Set<iw::Transform>(iw::vector3(0, 0, 2));
		text.Set<iw::Mesh>(t);

		back.Set<iw::Transform>(iw::vector3(0, 0, 1), 100);
		back.Set<iw::Mesh>(bg);

		sequence.Add([&]() {
			Bus->send<GameStateEvent>(SOFT_PAUSE);
			return true;
		});
		sequence.Add<iw::Delay>(1.0f);
		sequence.Add<iw::FadeValue<float>>(t .Material()->Get<float>("color") + 3, 1.0f, 0.5f);
		sequence.Add<iw::Delay>(2.0f);
		sequence.Add<iw::FadeValue<float>>(t .Material()->Get<float>("color") + 3, 0.0f, 1);
		sequence.Add<iw::FadeValue<float>>(bg.Material()->Get<float>("color") + 3, 0.0f, 0.5f);
		sequence.Add<iw::DestroyEntity>(text, true);
		sequence.Add<iw::DestroyEntity>(back, true);
		sequence.Add([&]() {
			Bus->send<GameStateEvent>(SOFT_RUN);
			return true;
		});
	}

	else if (currentLevelName == "levels/forest/forest02.json") {
		Space->DestroyEntity(otherGuy.Index());
		otherGuy = Space->CreateEntity<iw::Transform, iw::Mesh, iw::CollisionObject, iw::SphereCollider, OtherGuyTag>();

		otherGuy.Set<OtherGuyTag>(2);

		iw::Transform* t = otherGuy.Set<iw::Transform>(
			iw::vector3(-18, 1, -15),
			0
		);

		otherGuy.Set<iw::Mesh>(Asset->Load<iw::Model>("Sphere")->GetMesh(0).MakeInstance());

		iw::CollisionObject* obj = otherGuy.Set<iw::CollisionObject>();
		iw::SphereCollider*  col = otherGuy.Set<iw::SphereCollider>(0, 0.75f);

		obj->SetOnCollision([&](iw::Manifold& man, float dt) {
			iw::Entity guyEntity, bulletEntity;
			bool noent = GetEntitiesFromManifold<OtherGuyTag, Bullet>(man, guyEntity, bulletEntity);

			if (noent) {
				return;
			}

			OtherGuyTag* guy  = guyEntity.Find<OtherGuyTag>();
			iw::Mesh*    mesh = guyEntity.Find<iw::Mesh>();

			guy->Health -= 1;

			if (guy->Health < 0) guy->Health = 0;

			float color = guy->Health / 3.0f;
			mesh->Material()->Set("baseColor", iw::Color(0.8f, color, color, 1));

			Audio->AsStudio()->CreateInstance("playerDamaged");
		});

		obj->SetTrans(t);
		obj->SetCol(col);

		Physics->AddCollisionObject(obj);

		float speed = 6;

		sequence.Add([&]() {
			Bus->send<GameStateEvent>(SOFT_PAUSE);
			Bus->send<SetCameraTargetEvent>(iw::vector3(-5, 17.15f, -2));
			return true;
		});
		sequence.Add<iw::MoveToTarget> (otherGuy, iw::Transform(0, 0.75), 5, false, true);
		sequence.Add<iw::MoveToTarget> (otherGuy, iw::vector3(-16, 1, -4), speed);
		sequence.Add<iw::MoveToTarget> (otherGuy, iw::vector3(-10, 1, -7), speed);
		sequence.Add<iw::MoveToTarget> (otherGuy, iw::vector3( -7, 1, -3.25f), speed);
		sequence.Add<iw::PlaySound>("swordAttack");
		sequence.Add<iw::MoveToTarget> (otherGuy, iw::vector3( -5, 1, -3), speed * 7.5f);
		sequence.Add<iw::PlaySound>("enemyDeath");
		sequence.Add<iw::DestroyEntity>(firstEnemy);
		sequence.Add<iw::MoveToTarget> (otherGuy, iw::vector3( -3, 1, -2.75f), speed * 3.5f);
		sequence.Add<iw::MoveToTarget> (otherGuy, iw::vector3(  0.5f, 1,  1.5f), speed);
		sequence.Add<iw::MoveToTarget> (otherGuy, iw::Transform(0, 0), 1.5f, false, true);
		sequence.Add([&]() {
			Bus->send<GameStateEvent>(SOFT_RUN);
			Bus->send<SetCameraTargetEvent>(0, true);
			return true;
		});
		sequence.Add<iw::DestroyEntity>(otherGuy, true);
	}

	else if (currentLevelName == "levels/forest/forest02.json") {
	iw::Mesh t = Asset->Load<iw::Font>("fonts/Arial.fnt")->GenerateMesh("A lone soldier longs for his king...", .005f, 1);
	t.SetMaterial(Asset->Load<iw::Material>("materials/Font")->MakeInstance());
	t.Material()->Set("color", iw::Color(1, 1, 1, 0));

	iw::Entity text = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();

	text.Set<iw::Transform>(iw::vector3(0, 0, 2));
	text.Set<iw::Mesh>(t);

	sequence.Add([&]() {
		Bus->send<GameStateEvent>(SOFT_PAUSE);
		return true;
	});
	sequence.Add<iw::Delay>(1.0f);
	sequence.Add<iw::FadeValue<float>>(t.Material()->Get<float>("color") + 3, 1.0f, 0.5f);
	sequence.Add<iw::Delay>(2.0f);
	sequence.Add<iw::FadeValue<float>>(t.Material()->Get<float>("color") + 3, 0.0f, 1);
	sequence.Add<iw::DestroyEntity>(text, true);
	sequence.Add([&]() {
		Bus->send<GameStateEvent>(SOFT_RUN);
		return true;
	});
	}

	return level;
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
