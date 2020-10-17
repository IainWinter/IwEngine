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

#include "iw/graphics/ParticleSystem.h"

struct OtherGuyTag {
	int Health = 3;
};

LevelSystem::LevelSystem()
	: iw::System<iw::CollisionObject, iw::Model, LevelDoor>("Level")
{
	currentLevelName = "levels/canyon/canyon01.json";

	openColor   = iw::Color::From255(66, 201, 66, 63);
	closedColor = iw::Color::From255(201, 66, 66, 63);

	transition = false;
}

std::unordered_map<std::string, iw::StaticPS*> pSystems;

int LevelSystem::Initialize() {
	iw::Entity e = Space->CreateEntity<iw::Transform>();
	m_worldTransform = e.Set<iw::Transform>();

	// Leaves

	iw::ref<iw::Shader> particleShader = Asset->Load<iw::Shader>("shaders/particle/simple.shader");
	Renderer->InitShader(particleShader, iw::SHADOWS | iw::LIGHTS | iw::CAMERA);

	iw::Mesh& leafMesh = Asset->Load<iw::Model>("models/forest/redleaf.gltf")->GetMesh(0);
	leafMesh.Material()->SetShader(particleShader);
	leafMesh.Material()->SetTexture("shadowMap", Asset->Load<iw::Texture>("SunShadowMap"));
	leafMesh.Material()->Set("baseColor", iw::Color(1, 1, 1));

	//Bus->send<ResetLevelEvent>();

	return 0;
}

float startTime = 0;
iw::vector3 lastLevelPosition = 0;

void LevelSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [object, model, door] = entity.Components.Tie<Components>();

		object->Trans().Rotation *= iw::quaternion::from_euler_angles(iw::Time::DeltaTimeScaled() * .1f);

		if (door->ColorTimer > 0) {
			door->ColorTimer -= iw::Time::DeltaTimeScaled();

			iw::Color a, b; 

			if (door->State == LevelDoorState::OPEN) {
				a = closedColor;
				b = openColor;
			}

			else {
				a = openColor;
				b = closedColor;
			}

			model->GetMesh(0).Material()->Set("baseColor", iw::lerp<iw::vector4>(a, b, 4 * (0.25f - door->ColorTimer)));
		}
	}

	sequence.Update();
}

bool LevelSystem::On(
	iw::CollisionEvent& e)
{
	iw::Entity doorEntity, playerEntity;
	if (GetEntitiesFromManifold<LevelDoor, Player>(e.Manifold, doorEntity, playerEntity)) {
		return false;
	}

	LevelDoor*     door = doorEntity.Find<LevelDoor>();
	iw::Transform* tran = doorEntity.Find<iw::Transform>();

	if (!door) {
		LOG_INFO << "bad query";
		return true;
	}

	if (door->State == LevelDoorState::OPEN) {
		door->State = LevelDoorState::LOCKED; // stops events from being spammed
		//Bus->push<LoadNextLevelEvent>(door->NextLevel, tran->Position, door->GoBack);

		Bus->push<GotoConnectedLevelEvent>(door->Index);
	}

	return false;
}

bool LevelSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::UNLOCK_LEVEL_DOOR): {
			UnlockLevelDoorEvent& event = e.as<UnlockLevelDoorEvent>();

			Space->Query<LevelDoor>().Each([&](auto, auto door) {
				if (door->State != event.State) {
					door->State = event.State;
					door->ColorTimer = 0.25f;
				}
			});

			break;
		}
		case iw::val(Actions::LOAD_LEVEL): {
			LoadLevelEvent& event = e.as<LoadLevelEvent>();
			LoadLevel(event.LevelName, event.PreviousName);

			break;
		}

		case iw::val(Actions::UNLOAD_LEVEL): {
			std::string levelName = e.as<UnloadLevelEvent>().LevelName;

			if (levelName == "All") {
				for (auto pair : m_loadedLevels) {
					DeactivateLevel(pair.first);
					Space->QueueEntity(pair.second.first, iw::func_Destroy);
				}

				m_loadedLevels.clear();
			}

			else {
				auto itr = m_loadedLevels.find(levelName);
				if (itr != m_loadedLevels.end()) {
					DeactivateLevel(itr->first);
					Space->QueueEntity(itr->second.first, iw::func_Destroy);
					m_loadedLevels.erase(itr);
				}
			}

			break;
		}
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();

			auto [entity, level] = m_loadedLevels.at(event.LevelName);

			event.CameraFollow   = level.CameraFollow;
			event.PlayerPosition = -m_worldTransform->Position + level.InPosition;
			event.Level = Space->FindComponent<iw::Transform>(entity);

			break;
		}
		case iw::val(Actions::ACTIVATE_LEVEL): {
			ActivateLevelEvent& event = e.as<ActivateLevelEvent>();

			auto itr = m_loadedLevels.find(event.LevelName);
			if (itr != m_loadedLevels.end()) {

				Level& level = itr->second.second;

				ActivateLevel(event.LevelName);
				
				iw::vector3 target;

				if (event.Direction == 101) {
					m_worldTransform->Position = 0;
				}

				else if (event.Direction > 0) {
					m_previousLevelLocation = m_worldTransform->Position;
					target = m_previousLevelLocation - itr->second.second.LevelPosition;
				}

				else if (event.Direction < 0) {
					target = m_previousLevelLocation;
				}

				else {
					Bus->push<StartLevelEvent>(event.LevelName); // if reset just start & exit
					break;
				}

				Bus->push<TransitionToLevelEvent>(event.LevelName, 
					level.CameraFollow, -target + level.InPosition, level.LevelPosition);

				float start = iw::TotalTime();                  // if not transition then start
				float wait  = 1.0f;

				Task->queue([=]() {
					while (iw::TotalTime() - start < wait) {
						m_worldTransform->Position = iw::lerp(
							m_previousLevelLocation,
							target,
							iw::TotalTime() - start);
					}

					m_worldTransform->Position = target;

					Bus->push<StartLevelEvent>(event.LevelName);
				});
			}

			break;
		}
		case iw::val(Actions::DEACTIVATE_LEVEL): {
			std::string& name = e.as<ActivateLevelEvent>().LevelName;
			if (m_loadedLevels.find(name) != m_loadedLevels.end()) {
				DeactivateLevel(name);
			}

			break;
		}
	}

	return false;
}

std::pair<iw::EntityHandle, Level> LevelSystem::LoadLevel(
	const std::string& name,
	const std::string& from)
{
	srand(298374);

	iw::Entity levelEntity;
	Level level;

	iw::Transform* levelTransform = nullptr;
	bool reset = false;

	auto itr = m_loadedLevels.find(name);
	if (itr != m_loadedLevels.end()) {
		levelTransform = Space->FindComponent<iw::Transform>(itr->second.first);
		reset = true;
	}

	if (!reset) {
		iw::JsonSerializer("assets/" + name).Read(level);
		
		// Models

		for (ModelPrefab& prefab : level.Models) {
			iw::ref<iw::Model> model = Asset->Load<iw::Model>(prefab.ModelName);

			if (!model) {
				LOG_ERROR << "Failed to load part of level;";
				continue;
			}

			iw::Entity entity = Space->CreateEntity<iw::Transform, iw::Model>();

			iw::Transform* t = entity.Set<iw::Transform>(prefab.Transform);
		
			if (levelTransform) {
				levelTransform->AddChild(t);
			}

			else {
				levelTransform = t;
				levelEntity = entity;
			}

			for (int i = 0; i < model->MeshCount(); i++) {
				iw::Mesh&      mesh      = model->GetMesh(i);
				iw::Transform& transform = model->GetTransform(i);

				mesh.SetMaterial(mesh.Material()->MakeInstance());

				mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/vct/vct.shader"));
			
				if (mesh.Data()->Name().find("Bush") != std::string::npos) {
					mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/phong.shader"));
					//mesh.Material()->Set("baseColor", iw::Color(0.7, 0.7, 0.6, 1));
					//mesh.Material()->SetTransparency(iw::Transparency::ADD);
				}

				mesh.Material()->SetTexture("shadowMap", Asset->Load<iw::Texture>("SunShadowMap"));
				//mesh.Material()->SetTexture("shadowMap2", Asset->Load<iw::Texture>("LightShadowMap")); // shouldnt be part of material

				mesh.Material()->Set("indirectDiffuse", 1);
				mesh.Material()->Set("indirectSpecular", 0);

				if (mesh.Data()->Name().find("Ground") != std::string::npos) {
					if (mesh.Data()->Description().HasBuffer(iw::bName::COLOR)) {
						mesh.Material()->SetTexture("diffuseMap2", Asset->Load<iw::Texture>("textures/dirt/baseColor.jpg"));
						mesh.Material()->SetTexture("normalMap2", Asset->Load<iw::Texture>("textures/dirt/normal.jpg"));
					}
				}
			
				else if(mesh.Data()->Name().find("Tree") != std::string::npos) {
					auto itr = pSystems.find(mesh.Data()->Name());
					if (itr == pSystems.end()) {
						iw::StaticPS* ps = new iw::StaticPS();
					
						iw::Mesh leafMesh = Asset->Load<iw::Model>("models/forest/redleaf.gltf")->GetMesh(0);
						//leafMesh.SetData(leafMesh.Data()->MakeLink());

						ps->SetParticleMesh(leafMesh);

						iw::vector3* positions = (iw::vector3*)mesh.Data()->Get(iw::bName::POSITION); // should only do this once and then use a model matrix in the particle shader
						iw::vector3* normals   = (iw::vector3*)mesh.Data()->Get(iw::bName::NORMAL);
						iw::Color*   colors    = (iw::Color*)  mesh.Data()->Get(iw::bName::COLOR);

						unsigned count = mesh.Data()->GetCount(iw::bName::COLOR);

						for (int i = 0; i < count; i++) {
							if (   colors[i].r > 0.5f
								&& iw::randf() > 0.5f)
							{
								iw::vector3 rand = iw::vector3(iw::randf(), iw::randf(), iw::randf()) * iw::Pi;

								iw::Transform trans;
								trans.Position = positions[i] + rand * 0.2f;
								trans.Scale    = iw::randf() + 1.2f;
								trans.Rotation = iw::quaternion::from_euler_angles(iw::vector3(iw::Pi + rand.x * 0.2f, rand.y, rand.z * 0.2f));

								ps->SpawnParticle(trans);
							}
						}

						ps->UpdateParticleMesh();

						itr = pSystems.emplace(mesh.Data()->Name(), ps).first;
					}

					iw::Entity leaves = Space->CreateEntity<iw::Transform, iw::Mesh>();
				
					iw::Transform* t2 = leaves.Set<iw::Transform>(transform);
										leaves.Set<iw::Mesh>(itr->second->GetParticleMesh());

					t2->SetParent(t);
				}
			}

			entity.Set<iw::Model>(*model);
		}

		m_worldTransform->AddChild(levelTransform);

		m_loadedLevels.emplace(name, std::make_pair(levelEntity.Handle, level));
	}

	else {
		level = itr->second.second;
	}

	if (from.length() > 0) {
		iw::Transform* prev = Space->FindComponent<iw::Transform>(m_loadedLevels.at(from).first);
		levelTransform->Position = prev->Position + level.LevelPosition;
	}

	return { levelEntity.Handle, level };
}

void LevelSystem::ActivateLevel(
	const std::string& name)
{
	auto [handle, level] = m_loadedLevels.at(name);

	iw::Transform* levelTransform = Space->FindComponent<iw::Transform>(handle);

	// Colliders

	for (iw::PlaneCollider& prefab : level.Planes) {
		iw::Entity ent = Space->CreateEntity<iw::Transform, iw::PlaneCollider, iw::CollisionObject>();
		
		iw::Transform*       transform = ent.Set<iw::Transform>();
		iw::PlaneCollider*   collider  = ent.Set<iw::PlaneCollider>(prefab);
		iw::CollisionObject* object    = ent.Set<iw::CollisionObject>();

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);

		Physics->AddCollisionObject(object);
	}

	for (iw::CapsuleCollider& prefab : level.Capsules) {
		iw::Entity ent = Space->CreateEntity<iw::Transform, iw::CapsuleCollider, iw::CollisionObject>();

		iw::Transform*       transform = ent.Set<iw::Transform>(iw::vector3::unit_y);
		iw::CapsuleCollider* collider  = ent.Set<iw::CapsuleCollider>(prefab);
		iw::CollisionObject* object    = ent.Set<iw::CollisionObject>();

		levelTransform->AddChild(transform);

		object->SetCol(collider);
		object->SetTrans(transform);

		Physics->AddCollisionObject(object);
	}

	for (iw::SphereCollider& prefab : level.Spheres) {
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

	for (size_t i = 0; i < level.Doors.size(); i++) {
		iw::Entity ent = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::CollisionObject, LevelDoor>();
	
		if (i == 0) {
			nextLevelDoor = ent;
		}

		LevelDoor*           door      = ent.Set<LevelDoor>(level.Doors[i]);
		iw::Model*           model     = ent.Set<iw::Model>(*Asset->Load<iw::Model>("Door"));
		iw::Transform*       transform = ent.Set<iw::Transform>(iw::vector3(level.DoorPositions[i].x, 1, level.DoorPositions[i].y), 5.0f);
		iw::SphereCollider*  collider  = ent.Set<iw::SphereCollider>(iw::vector3::zero, 1.0f);
		iw::CollisionObject* object    = ent.Set<iw::CollisionObject>();

		// temp
		if (door->GoBack) {
			door->Index = -1;
		}

		else {
			door->Index = i + 1;
		}

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

	// Enemies

	firstEnemy = iw::Entity();

	for (size_t i = 0; i < level.Enemies.size(); i++) {
		iw::vector3 position;
		position.x = level.Positions[i].x;
		position.z = level.Positions[i].y;
		position.y = 1;

		Bus->send<SpawnEnemyEvent>(level.Enemies[i], position, 0, levelTransform);

		if (!firstEnemy) {
			firstEnemy = Space->FindEntity(levelTransform->Children().back());
		}
	}

	// should look into serialization of prefabs to not have to do this

	// Spawning items
	
	sequence = CreateSequence();

	if (name == "levels/forest/forest05.a.json") {
		Bus->push<SpawnItemEvent>(Item{ NOTE,       0 }, iw::vector3(3, 1, -2), levelTransform);
		Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 0 }, iw::vector3(0, 1, 3),  levelTransform);
	}

	else if (name == "levels/forest/forest07.a.json") {
		Bus->push<SpawnItemEvent>(Item{ NOTE,       1 }, iw::vector3(24, 1, 8), levelTransform);
		Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 0 }, iw::vector3(8, 1, 0), levelTransform);
	}

	else if (name == "levels/forest/forest12.a.json") {
		Bus->push<SpawnItemEvent>(Item{ NOTE,       2 }, iw::vector3(3, 1, -2), levelTransform);
		Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 1 }, iw::vector3(0, 1,  3), levelTransform);
	}

	else if (name == "levels/canyon/cave04.json") {
		Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 2 }, iw::vector3(22, 1, 7.5f), levelTransform);
	}

	else if (name == "levels/canyon/cave06.json") {
		Bus->push<SpawnItemEvent>(Item{ CONSUMABLE, 2 }, iw::vector3(4, 1, -9.5f), levelTransform);
	}

	// run a cut scene

	if (name == "levels/forest/forest01.json") {
		iw::Mesh t = Asset->Load<iw::Font>("fonts/Arial.fnt")->GenerateMesh("A lone soldier longs for his king...", .005f, 1);
		t.SetMaterial(Asset->Load<iw::Material>("materials/Font")->MakeInstance());
		t.Material()->Set("color", iw::Color(0, 0, 0, 1));
		t.Material()->SetTransparency(iw::Transparency::ADD);

		iw::ref<iw::Shader> shader = Asset->Load<iw::Shader>("shaders/simple.shader");
		Renderer->InitShader(shader, iw::CAMERA);

		iw::Material material(shader);
		material.Set("color", iw::Color(0, 0, 0, 1));
		material.SetTransparency(iw::Transparency::ADD);

		iw::Mesh bg = Renderer->ScreenQuad().MakeInstance();
		bg.SetMaterial(REF<iw::Material>(material));

		iw::Entity text = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();
		iw::Entity back = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();

		text.Set<iw::Transform>(iw::vector3(0, 0, 1));
		text.Set<iw::Mesh>(t);

		back.Set<iw::Transform>(iw::vector3(0, 0, -2), 100);
		back.Set<iw::Mesh>(bg);

		sequence.Add([&]() {
			Bus->send<GameStateEvent>(SOFT_PAUSE);
			return true;
		});
		sequence.Add<iw::Delay>(1.0f);
		sequence.Add<iw::FadeValue<iw::vector4>>(t.Material()->Get<iw::vector4>("color"), iw::vector4(1), 0.5f);
		sequence.Add<iw::Delay>(2.0f);
		sequence.Add<iw::FadeValue<iw::vector4>>(t.Material()->Get<iw::vector4>("color"), iw::vector4(0, 0, 0, 1), 1);
		sequence.Add<iw::FadeValue<float>>(t.Material()->Get<float>("color") + 3, 0.0f, 1);
		sequence.Add<iw::FadeValue<float>>(bg.Material()->Get<float>("color") + 3, 0.0f, 0.5f);
		sequence.Add<iw::DestroyEntity>(text, true);
		sequence.Add<iw::DestroyEntity>(back, true);
		sequence.Add([&]() {
			Bus->send<GameStateEvent>(SOFT_RUN);
			return true;
		});
	}

	else if (name == "levels/forest/forest02.json") {
		Space->DestroyEntity(otherGuy.Index());
		otherGuy = Space->CreateEntity<iw::Transform, iw::Model, iw::CollisionObject, iw::SphereCollider, OtherGuyTag>();

		otherGuy.Set<OtherGuyTag>(2);

		iw::Transform* t = otherGuy.Set<iw::Transform>(
			iw::vector3(-18, 1, -15),
			0
		);

		iw::Model* model = otherGuy.Set<iw::Model>(*Asset->Load<iw::Model>("Sphere"));

		model->GetMesh(0).Material()->Set("baseColor", iw::vector4(0.8f, 1.0f));

		iw::CollisionObject* obj = otherGuy.Set<iw::CollisionObject>();
		iw::SphereCollider*  col = otherGuy.Set<iw::SphereCollider>(0, 0.75f);

		obj->SetOnCollision([&](iw::Manifold& man, float dt) {
			iw::Entity guyEntity, bulletEntity;
			bool noent = GetEntitiesFromManifold<OtherGuyTag, Bullet>(man, guyEntity, bulletEntity);

			if (noent) {
				return;
			}

			OtherGuyTag* guy  = guyEntity.Find<OtherGuyTag>();
			iw::Mesh&    mesh = guyEntity.Find<iw::Model>()->GetMesh(0);

			guy->Health -= 1;

			if (guy->Health < 0) guy->Health = 0;

			float color = guy->Health / 3.0f;
			mesh.Material()->Set("baseColor", iw::Color(0.8f, color, color, 1));

			Audio->AsStudio()->CreateInstance("User/playerDamaged");
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

	if (name == "levels/forest/forest07.json") {
		iw::Mesh t = Asset->Load<iw::Font>("fonts/Arial.fnt")->GenerateMesh("Oh look it seems we've missed one...", .003f, 1);
		t.SetMaterial(Asset->Load<iw::Material>("materials/Font")->MakeInstance());
		t.Material()->Set("color", iw::Color(1, 1, 1, 0));
		t.Material()->SetTransparency(iw::Transparency::ADD);

		iw::Entity text = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();

		iw::vector3 pos = firstEnemy.Find<iw::Transform>()->Position;
		pos.y = pos.z + .5f;
		pos.z = -5;
		
		pos.x /= 32;
		pos.x *= 9;
		pos.x -= 0.5f;

		text.Set<iw::Transform>(pos);
		text.Set<iw::Mesh>(t);

		sequence.Add([&]() {
			Bus->send<GameStateEvent>(SOFT_PAUSE);
			Bus->send<GameStateEvent>(PAUSED);
			return true;
		});
		sequence.Add<iw::Delay>(0.1f);
		sequence.Add<iw::FadeValue<float>>(t.Material()->Get<float>("color") + 3, 1.0f, 2);
		sequence.Add<iw::Delay>(1.5f);
		sequence.Add<iw::FadeValue<float>>(t.Material()->Get<float>("color") + 3, 0.0f, 1.5f);
		sequence.Add<iw::DestroyEntity>(text, true);
		sequence.Add([&]() {
			Bus->send<GameStateEvent>(RUNNING);
			Bus->send<GameStateEvent>(SOFT_RUN);
			return true;
		});
	}

	else if (name == "levels/forest/forest08.json") {
		iw::ref<iw::Material> material = Asset->Load<iw::Material>("materials/Font")->MakeInstance();
		material->Set("color", iw::Color(1, 1, 1, 0));
		material->SetTransparency(iw::Transparency::ADD);
		
		iw::Mesh t = Asset->Load<iw::Font>("fonts/Arial.fnt")->GenerateMesh("Who are you?\nHow'd you get passed my guards?!", .003f, 1);
		t.SetMaterial(material);

		iw::Mesh t2 = Asset->Load<iw::Font>("fonts/Arial.fnt")->GenerateMesh("Wait no my men!! I'll never forgive you!!!", .003f, 1);
		t2.SetMaterial(material->MakeInstance());

		iw::Entity text  = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();
		iw::Entity text2 = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement>();

		iw::vector3 pos = firstEnemy.Find<iw::Transform>()->Position;
		pos.y = pos.z + .5f;
		pos.z = -5;

		pos.x /= 32;
		pos.x *= 9;
		pos.x -= 0.5f;

		text.Set<iw::Transform>(pos);
		text.Set<iw::Mesh>(t);

		text2.Set<iw::Transform>(pos);
		text2.Set<iw::Mesh>(t2);

		sequence.Add([&]() {
			Bus->send<GameStateEvent>(SOFT_PAUSE);
			Bus->send<GameStateEvent>(PAUSED);
			return true;
		});
		sequence.Add<iw::Delay>(0.1f);
		sequence.Add<iw::FadeValue<float>>(t.Material()->Get<float>("color") + 3, 1.0f, 2);
		sequence.Add<iw::Delay>(1.5f);
		sequence.Add<iw::FadeValue<float>>(t.Material()->Get<float>("color") + 3, 0.0f, 1.5f);
		sequence.Add<iw::DestroyEntity>(text, true);

		sequence.Add<iw::FadeValue<float>>(t2.Material()->Get<float>("color") + 3, 1.0f, 2);
		sequence.Add<iw::Delay>(1.5f);
		sequence.Add<iw::FadeValue<float>>(t2.Material()->Get<float>("color") + 3, 0.0f, 1.5f);
		sequence.Add<iw::DestroyEntity>(text2, true);

		sequence.Add([&]() {
			Bus->send<GameStateEvent>(RUNNING);
			Bus->send<GameStateEvent>(SOFT_RUN);
			return true;
		});
	}
}

void LevelSystem::DeactivateLevel(
	const std::string& name)
{
	auto [handle, level] = m_loadedLevels.at(name);

	iw::Transform* levelTransform = Space->FindComponent<iw::Transform>(handle);

	auto func = [&](auto e, auto transform, auto) {
		if (transform->Parent() == levelTransform) {
			Space->QueueEntity(e, iw::func_Destroy);
		}
	};

	Space->Query<iw::Transform, LevelDoor>().Each(func);
	Space->Query<iw::Transform, iw::CollisionObject>().Each(func);
	Space->Query<iw::Transform, iw::Rigidbody>().Each(func);
}

void LevelSystem::DestroyAll(
	iw::Transform* transform) // dont use this, somehow some physics objects loose their parents?? 
{
	if (transform) {
		for (iw::Transform* child : transform->Children()) {
			DestroyAll(child);
		}

		iw::Entity e = Space->FindEntity(transform);
		if (e) {
			e.Destroy();
		}
	}
}
