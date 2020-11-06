#include "Systems/SpecialBarrierSystem.h"

#include "Components/DontDeleteBullets.h"
#include "Components/WorldHole.h"
#include "Components/Player.h"
#include "Events/ActionEvents.h"

#include "iw/common/Components/Transform.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/graphics/Model.h"
#include "iw/events/binding.h"

SpecialBarrierSystem::SpecialBarrierSystem(
	iw::Entity& player)
	: iw::SystemBase("Special Barrier")
	, player(player)
	, dashThrough(false)
{}

// sucks that this needs to be here, need to fix lambdas to get it to work
void SpecialBarrierSystem::collide(
	iw::Manifold& man,
	iw::scalar dt)
{
	iw::Entity barrierEntity, playerEntity;
	if (GetEntitiesFromManifold<DontDeleteBullets, Player>(man, barrierEntity, playerEntity)) {
		return;
	}

	if (man.PenetrationDepth > 1) {
		man.HasCollision = false;

		if (   !dashThrough
			&& playerEntity.Find<Player>()->Timer <= 0)
		{
			iw::Rigidbody* body = playerEntity.Find<iw::Rigidbody>();

			body->SetSimGravity(true);
			body->SetIsLocked(0);
			body->SetCol(nullptr);

			playerEntity.Find<Player>()->Speed = 0;

			float start = iw::TotalTime();
			float wait = 2.00f;

			Task->queue([=]() {
				while (iw::TotalTime() - start < wait) {
					// spin lock
				}

				Bus->push<ResetLevelEvent>();
			});
		}
	}
}

void SpecialBarrierSystem::Update() {
	auto query = Space->MakeQuery<iw::CollisionObject, DontDeleteBullets>();
	query->SetNone({ Space->GetComponent<WorldHole>() });

	for(auto entity : Space->Query(query)) {
		iw::CollisionObject* object = entity.Components.Get<iw::CollisionObject, 0>();

		if (   dashThrough
			&& player.Find<Player>()->Timer > 0.0f)
		{
			object->SetIsTrigger(true);
		}

		else {
			object->SetIsTrigger(false);
		}
	}
}

bool SpecialBarrierSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::LONG_DASH_ACTIVE): {
			dashThrough = e.as<LongDashEvent>().Active;
			break;
		}
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();

			if (event.LevelName == "levels/canyon/cave04.json") {
				event.Level->AddChild(SpawnSpecialBarrier({iw::vector3(0.6f, 0, -0.2f), iw::vector3::unit_x, 29.8f, 2.2f }));
			}

			else if (event.LevelName == "levels/canyon/cave06.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(6.4f, 0, -4.4f), iw::vector3::unit_x, 60, 2 }));
			}

			else if (event.LevelName == "levels/canyon/cave07.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 2.2f, 0, -1.6f),  iw::vector3::unit_x,  6.7f, 0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 2.7f, 0,  1.6f),  iw::vector3::unit_x,  7.4f, 0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 0.5f, 0, 14   ),  iw::vector3::unit_z, 23.8f, 9    }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(10.7f, 0,  10.9f), iw::vector3::unit_x, 24.4f, 5.8f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 6.1f, 0,  3.5f),  iw::vector3::unit_z,  4,    0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 5.3f, 0, -6.6f),  iw::vector3::unit_z, 10.6f, 0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-1,    0, -6.6f),  iw::vector3::unit_z, 10.6f, 0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 2.1,  0, -6.6f),  iw::vector3::unit_z, 10.0f, 2.7f }));
			}

			else if (event.LevelName == "levels/river/river02.json"
				  || event.LevelName == "levels/river/river03.json"
				  || event.LevelName == "levels/river/river08.json")
			{
				event.Level->AddChild(SpawnSpecialBarrier({ 0, iw::vector3::unit_x, 100, 8 }));
			}

			else if (event.LevelName == "levels/river/river04.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-16.6f, 0, 0.7f),  iw::vector3::unit_x, 35.9f, 7.5f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-8.0f, 0, -7.3f),  iw::vector3::unit_z, 17.3f, 4.0f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-8.0f, 0, -13.0f), iw::vector3::unit_x, 13.0f, 3.5f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-2.2f, 0, -16.1f), iw::vector3::unit_x, 31.0f, 5.0f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(8.5f, 0, -15.0f),  iw::vector3::unit_x,  15.0f, 4.5f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(14.0f, 0, -12.0f), iw::vector3::unit_x, 13.5f, 4.0f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(18.0f, 0, -8.0f),  iw::vector3::unit_x, 13.0f, 4.0f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(28.0f, 0, -3.5f),  iw::vector3::unit_x, 25.0f, 8.8f }));
			}

			else if (event.LevelName == "levels/river/river05.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-18, 0, 0),    iw::vector3::unit_x, 50, 8 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 27, 0, 0),    iw::vector3::unit_x, 30, 8 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 15.5f, 0, 0), iw::vector3::unit_z, 15, 4.5f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 3.7f,  0, 0), iw::vector3::unit_z, 15, 4.5f }));
			}

			else if (event.LevelName == "levels/river/river06.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ 0, iw::vector3::unit_x, 100, 8 }));
				
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-9.4f, 0, 8.4f),  iw::vector3::unit_x, 11, 2 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 5.7f, 0, 18.6f), iw::vector3::unit_x, 31, 2.9f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-7.5f, 0, 16.2f), iw::vector3::unit_x, 12, 3.2f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(19.0f, 0, 28.0f), iw::vector3::unit_x, 22.1f, 6.5f }));
			}

			else if (event.LevelName == "levels/river/river07.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ 0, iw::vector3::unit_x, 100, 8 }));

				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 0, 0, -6.5f), iw::vector3::unit_z, 21, 8 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-4, 0, -36),   iw::vector3::unit_z, 30, 8 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-3, 0, -25),   iw::vector3::unit_x, 15, 4.5f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-2, 0, -13),   iw::vector3::unit_x, 15, 4.5f }));
			}

			else if (event.LevelName == "levels/river/river09.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 15, 0, 0),   iw::vector3::unit_x, 50, 8 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-30, 0, 0),    iw::vector3::unit_x, 30, 8 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-18.5f, 0, 0), iw::vector3::unit_z, 15, 4.5f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-6.6f,  0, 0), iw::vector3::unit_z, 15, 4.5f }));
			}

			else if (event.LevelName == "levels/river/river10.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-21.0f, 0, 0), iw::vector3::unit_x, 50, 8 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 10.5f, 0, 0), iw::vector3::unit_x, 29, 8 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(5,  0, 7),     iw::vector3::unit_x, 15, 1 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(30, 0, 4.5f),  iw::vector3::unit_x, 22, 8.5f }));

				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-9, 0, -11),   iw::vector3::unit_z, 14.5f, 4.5f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(0, 0, -18.7f), iw::vector3::unit_x, 20, 2 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(10, 0, -7),    iw::vector3::unit_z, 22, 6 }));

				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-5, 0, -16.5f), iw::vector3::unit_x, 6.5f, 1 }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-6, 0, -7),     iw::vector3::unit_x, 12, 3 }));
			}

			else if (event.LevelName == "levels/river/river11.json") {

			}

			else if (event.LevelName == "levels/river/river12.json") {

			}

			break;
		}
	}

	return false;
}

iw::Transform* SpecialBarrierSystem::SpawnSpecialBarrier(
	iw::CapsuleCollider capsule)
{
	iw::Entity ent = Space->CreateEntity<iw::Transform, iw::CapsuleCollider, iw::CollisionObject, DontDeleteBullets>();

	iw::Transform*       transform = ent.Set<iw::Transform>(iw::vector3::unit_y);
	iw::CapsuleCollider* collider  = ent.Set<iw::CapsuleCollider>(capsule);
	iw::CollisionObject* object    = ent.Set<iw::CollisionObject>();

	object->SetCol(collider);
	object->SetTrans(transform);

	object->SetOnCollision(iw::bind<void, SpecialBarrierSystem*, iw::Manifold&, float>(&SpecialBarrierSystem::collide, this)); // garbo

	Physics->AddCollisionObject(object);

	return transform;
}
