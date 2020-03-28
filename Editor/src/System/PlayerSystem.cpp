#include "Systems/PlayerSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/audio/AudioSpaceStudio.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/reflection/serialization/JsonSerializer.h"
#include "iw/util/io/File.h"

PlayerSystem::PlayerSystem()
	: System<iw::Transform, iw::Rigidbody, Player>("Player")
	, playerPrefab()
	, m_playerModel(nullptr)
	, movement(0)
	, dash(false)
{
#ifdef IW_DEBUG
	const char* file = "assets/prefabs/player.json";
	bool fileExists  = iw::FileExists(file);
#else
	const char* file = "assets/prefabs/player.bin";
	bool fileExists  = iw::FileExists(file);
#endif

	if (fileExists) {
#ifdef IW_DEBUG
		iw::JsonSerializer(file).Read(playerPrefab);
#else
		iw::Serializer(file).Read(playerPrefab);
#endif
	}

	else {
		// Default values
		playerPrefab.Speed        = 4.0f;
		playerPrefab.DashTime     = 8 / 60.0f;
		playerPrefab.CooldownTime = 0.2f;
		playerPrefab.Health       = 3;

#ifdef IW_DEBUG
		iw::JsonSerializer(file).Write(playerPrefab);
#else
		iw::Serializer(file).Write(playerPrefab);
#endif
	}
}

int PlayerSystem::Initialize() {
	m_playerModel = Asset->Load<iw::Model>("Player");
	
	player = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Player>();
	
	                         player.SetComponent<iw::Model>(*Asset->Load<iw::Model>("Player"));
	                         player.SetComponent<Player>(playerPrefab);
	iw::Transform*      tp = player.SetComponent<iw::Transform>(iw::vector3::zero, iw::vector3(.75f));
	iw::SphereCollider* sp = player.SetComponent<iw::SphereCollider>(iw::vector3::zero, 1);
	iw::Rigidbody*      rp = player.SetComponent<iw::Rigidbody>();

	rp->SetMass(1);
	rp->SetCol(sp);
	rp->SetTrans(tp);
	rp->SetStaticFriction (0.0f);
	rp->SetDynamicFriction(0.0f);

	rp->SetIsLocked(iw::vector3(0, 1, 0));
	rp->SetLock    (iw::vector3(0, 1, 0));

	Physics->AddRigidbody(rp);

	return 0;
}

void PlayerSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, rigidbody, player] = entity.Components.Tie<Components>();

		if (player->DeathTimer > 0) {
			player->DeathTimer -= iw::Time::DeltaTime();

			if (player->DeathTimer <= 0) {
				player->DeathTimer = 0;
				Bus->push<ResetLevelEvent>();
			}
		}

		else {
			if (player->Timer <= -player->CooldownTime) {
				if (dash) {
					player->Timer = player->DashTime;
					Audio->AsStudio()->CreateInstance("swordAttack");
				}
			}

			else if (player->Timer >= -player->CooldownTime) {
				player->Timer -= iw::Time::DeltaTime();
			}

			if (player->Health <= 0) {
				if (player->DeathTimer == 0) {
					player->DeathTimer = 1.0f;

					//Audio->PlaySound("Death.wav");
				}
			}

			player->Damaged = false;
		}
	}
}

void PlayerSystem::FixedUpdate(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, rigidbody, player] = entity.Components.Tie<Components>();

		if (player->DeathTimer > 0) {
			rigidbody->SetVelocity(0);
			transform->Scale -= .75f / 1.0f * iw::Time::FixedTime();
		}

		else {
			iw::vector3 m = movement.normalized() * player->Speed;
			if (movement != 0) {
				if (player->Timer > 0) {
					m *= 10 * player->Timer / player->DashTime;
				}
			}

			iw::vector3 v = rigidbody->Velocity();
			v.x = m.x;
			v.z = m.z;

			rigidbody->SetVelocity(v);
		}
	}
}

bool PlayerSystem::On(
	iw::KeyEvent& event)
{
	switch (event.Button) {
		case iw::UP: {
			if (    event.State && movement.z == -1
				|| !event.State && movement.z ==  1)
			{
				movement.z = 0;
			}

			movement.z -= event.State ? 1 : -1;
			break;
		}
		case iw::DOWN: {
			if (    event.State && movement.z ==  1
				|| !event.State && movement.z == -1)
			{
				movement.z = 0;
			}

			movement.z += event.State ? 1 : -1;
			break; 
		}
		case iw::LEFT: {
			if (    event.State && movement.x == -1
				|| !event.State && movement.x ==  1)
			{
				movement.x = 0;
			}

			movement.x -= event.State ? 1 : -1;
			break; 
		}
		case iw::RIGHT: {
			if (    event.State && movement.x ==  1
				|| !event.State && movement.x == -1)
			{
				movement.x = 0;
			}

			movement.x += event.State ? 1 : -1;
			break; 
		}
		case iw::X: {
			dash = event.State;
			break;
		}
	}

	return true;
}

bool PlayerSystem::On(
	iw::CollisionEvent& event)
{
	iw::Entity a = Space->FindEntity(event.ObjA);
	if (a == iw::EntityHandle::Empty) {
		a = Space->FindEntity<iw::Rigidbody>(event.ObjA);
	}

	iw::Entity b = Space->FindEntity(event.ObjB);
	if (b == iw::EntityHandle::Empty) {
		b = Space->FindEntity<iw::Rigidbody>(event.ObjB);
	}

	if (   a.Index() == iw::EntityHandle::Empty.Index
		|| b.Index() == iw::EntityHandle::Empty.Index)
	{
		return false;
	}

	iw::Entity player;
	if (   a.HasComponent<Player>()
		&& b.HasComponent<Bullet>())
	{
		player = a;
	}

	else if (b.HasComponent<Player>()
		&&   a.HasComponent<Bullet>())
	{
		player = b;
	}

	if (player.Index() != iw::EntityHandle::Empty.Index) {
		Player* p = player.FindComponent<Player>();
		if (  !p->Damaged
			&& p->Health > 0)
		{
			p->Damaged = true;
			p->Health -= 1;

			float color = p->Health / 5.0f;
			m_playerModel->Meshes[0].Material->Set("albedo", iw::Color(1, color, color, 1));

			Audio->AsStudio()->CreateInstance("playerDamaged");
		}
	}

	return false;
}

bool PlayerSystem::On(
	iw::ActionEvent& event)
{
	switch (event.Action) {
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			iw::Transform* t = player.FindComponent<iw::Transform>();
			iw::Rigidbody* r = player.FindComponent<iw::Rigidbody>();

			r->SetVelocity(0);
			r->SetIsKinematic(false);

			t->Scale = 0;

			movement = 0;
			dash = 0;
			break;
		}
		case iw::val(Actions::START_LEVEL): {
			iw::Transform* t = player.FindComponent<iw::Transform>();
			iw::Rigidbody* r = player.FindComponent<iw::Rigidbody>();
			Player*        p = player.FindComponent<Player>();

			t->Position.x = event.as<StartLevelEvent>().PlayerPosition.x;
			t->Position.z = event.as<StartLevelEvent>().PlayerPosition.y;
			t->Scale = 0.75f;
			
			r->SetTrans(t);
			r->SetIsKinematic(true);

			*p = playerPrefab;

			// no break
		}
		case iw::val(Actions::RESET_LEVEL): {
			m_playerModel->Meshes[0].Material->Set("albedo", iw::Color(1));
			break;
		}
	}

	return false;
}
