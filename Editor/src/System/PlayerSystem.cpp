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
	: System("Player")
	, playerPrefab()
	, m_playerModel(nullptr)
	, up(false)
	, down(false)
	, left(false)
	, right(false)
	, dash(false)
	, sprint(false)
	, transition(false)
	, begin(0)
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
		playerPrefab.ChargeTime = 0.2f;
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
	
	player = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::CollisionObject, Player>();
	
	                         player.SetComponent<iw::Model>(*m_playerModel);
	                         player.SetComponent<Player>(playerPrefab);
	iw::Transform*       t = player.SetComponent<iw::Transform>(iw::vector3(0, 1, 0), iw::vector3(.75f));
	iw::SphereCollider*  s = player.SetComponent<iw::SphereCollider>(iw::vector3::zero, 1);
	iw::CollisionObject* c = player.SetComponent<iw::CollisionObject>();

	//c->SetMass(1);
	c->SetCol(s);
	c->SetTrans(t);
	c->SetIsStatic(false);
	//c->SetStaticFriction (0.0f);
	//c->SetDynamicFriction(0.0f);

	//c->SetIsLocked(iw::vector3(0, 1, 0));
	//c->SetLock    (iw::vector3(0, 1, 0));

	Physics->AddCollisionObject(c);

	return 0;
}

float distance; //tmp debug
iw::vector3 start;

void PlayerSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, object, player] = entity.Components.Tie<Components>();

		if (transition) {
			object->Trans().Position = iw::lerp(transitionStartPosition, transitionTargetPosition, 0.75f * (iw::Time::TotalTime() - begin));
		}

		else {
			if (player->DeathTimer > 0) {
				player->DeathTimer -= iw::Time::DeltaTime();

				transform->Scale -= .75f / 1.0f * iw::Time::DeltaTime();

				if (player->DeathTimer <= 0) {
					player->DeathTimer = 0;
					Bus->push<ResetLevelEvent>();
				}
			}

			else {
				iw::vector3 movement;
				if (left)  movement.x -= 1;
				if (right) movement.x += 1;
				if (up)    movement.z -= 1;
				if (down)  movement.z += 1;

				movement.normalize();
				movement *= player->Speed;

				if (player->Timer > 0) {
					movement *= 10 * player->Timer / player->DashTime;
				}

				if (sprint) {
					movement *= 2.0f;
				}

				object->Trans().Position += movement * iw::Time::DeltaTime();

				if (player->Timer <= -player->ChargeTime) {
					if (distance == 0) {
						distance = (start - transform->Position).length();
						LOG_INFO << distance;
					}

					if (dash
						&& (up || down || left || right))
					{
						start = transform->Position;
						distance = 0;

						player->Timer = player->DashTime;
						Audio->AsStudio()->CreateInstance("swordAttack");
					}
				}

				// sprinting after holding x?
				//if (player->Timer <= 0.0f) {
				//	if (dash
				//		|| !sprint)
				//	{
				//		sprint = dash;
				//	}
				//}

				else if (player->Timer >= -player->ChargeTime) {
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
}

bool PlayerSystem::On(
	iw::KeyEvent& event)
{
	switch (event.Button) {
		case iw::UP: {
			up = event.State;
			break;
		}
		case iw::DOWN: {
			down = event.State;
			break; 
		}
		case iw::LEFT: {
			left = event.State;
			break; 
		}
		case iw::RIGHT: {
			right = event.State;
			break; 
		}
		case iw::X: {
			dash = event.State;
			break;
		}
		case iw::R: {
			if (event.State) {
				Bus->send<ResetLevelEvent>();
			}

			break;
		}
	}

	return true;
}

bool PlayerSystem::On(
	iw::CollisionEvent& event)
{
	// i frames
	//Player* p = player.FindComponent<Player>();

	//if (p->Timer >= p->DashTime - 0.05f) {
	//	return false;
	//}

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

			float color = p->Health / 3.0f;
			m_playerModel->GetMesh(0).Material()->Set("albedo", iw::Color(1, color, color, 1));

			Audio->AsStudio()->CreateInstance("playerDamaged");
		}
	}

	return false;
}

bool PlayerSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			GoToNextLevelEvent& event = e.as<GoToNextLevelEvent>();

			iw::CollisionObject* c = player.FindComponent<iw::CollisionObject>();

			c->SetCol(nullptr);

			transition = true;
			transitionStartPosition  = iw::vector3(c->Trans().Position.x, 1, c->Trans().Position.z);
			transitionTargetPosition = iw::vector3(event.CenterPosition.x + event.PlayerPosition.x, 1, event.CenterPosition.y + event.PlayerPosition.y);
			begin = iw::Time::TotalTime();

			//t->Scale = 0;

			up = down = left = right = dash = sprint = false;

			break;
		}
		case iw::val(Actions::START_LEVEL): {
			iw::Transform*       t = player.FindComponent<iw::Transform>();
			iw::CollisionObject* c = player.FindComponent<iw::CollisionObject>();
			iw::SphereCollider*  s = player.FindComponent<iw::SphereCollider>();
			Player*              p = player.FindComponent<Player>();

			t->Position.x = e.as<StartLevelEvent>().PlayerPosition.x;
			t->Position.z = e.as<StartLevelEvent>().PlayerPosition.y;
			t->Scale = 0.75f;
			
			c->SetCol(s);
			c->SetTrans(t);

			*p = playerPrefab;

			transition = false;

			// no break
		}
		case iw::val(Actions::RESET_LEVEL): {
			m_playerModel->GetMesh(0).Material()->Set("albedo", iw::Color(1));
			break;
		}
	}

	return false;
}
