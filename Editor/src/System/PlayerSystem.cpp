#include "Systems/PlayerSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/audio/AudioSpaceStudio.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/reflection/serialization/JsonSerializer.h"
#include "iw/util/io/File.h"

#include "Components/Enemy.h"

PlayerSystem::PlayerSystem()
	: System("Player")
	, playerPrefab()
	, m_playerModel(nullptr)
	//, up(false)
	//, down(false)
	//, left(false)
	//, right(false)
	//, dash(false)
	//, sprint(false)
	//, transition(false)
	//, levelTransition(false)
	//, begin(0)
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
		playerPrefab.Speed      = 4.25f;
		playerPrefab.DashTime   = 8 / 60.0f;
		playerPrefab.ChargeTime = 0.2f;
		playerPrefab.Health     = 3;

#ifdef IW_DEBUG
		iw::JsonSerializer(file).Write(playerPrefab);
#else
		iw::Serializer(file).Write(playerPrefab);
#endif
	}
}

int PlayerSystem::Initialize() {
	m_playerModel = Asset->Load<iw::Model>("Player");

	m_player = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Player>();
	
	                        m_player.Set<Player>(playerPrefab);
	                        m_player.Set<iw::Model>(*m_playerModel);
	iw::Transform*      t = m_player.Set<iw::Transform>(iw::vector3(0, 1, 0), iw::vector3(1));
	iw::SphereCollider* s = m_player.Set<iw::SphereCollider>(iw::vector3::zero, 0.75f);
	iw::Rigidbody*      r = m_player.Set<iw::Rigidbody>();

	//c->SetMass(1);
	r->SetCol(s);
	r->SetTrans(t);
	r->SetIsStatic(false);
	r->SetSimGravity(false);

	r->SetIsLocked(iw::vector3::unit_y);
	r->SetLock    (iw::vector3::unit_y);

	r->SetOnCollision([&](iw::Manifold& man, float dt) {
		iw::Entity enemyEntity, playerEntity;
		bool noent = GetEntitiesFromManifold<Enemy, Player>(man, enemyEntity, playerEntity);

		if (noent) {
			return;
		}

		Enemy*  enemy  = enemyEntity .Find<Enemy>();
		Player* player = playerEntity.Find<Player>();

		if (player->Timer <= 0) {
			return;
		}

		if (enemy->Health > 1) {
			if (player->Transition) return; 

			Player*        playerComp  = playerEntity.Find<Player>();
			iw::Transform* playerTrans = playerEntity.Find<iw::Transform>();
			iw::Rigidbody* playerBody  = playerEntity.Find<iw::Rigidbody>();

			//playerComp->Timer = 0.0f; // causes the enemy to not detect damage
			playerBody->SetIsTrigger(true);

			QueueChange(&player->Transition, true);

			player->TransitionSpeed = 1.5f;
			player->TransitionStartPosition  = playerTrans->Position;
			player->TransitionTargetPosition = playerTrans->Position + player->Movement.normalized() * 12;
			player->Begin = iw::Time::TotalTime();
		}
	});

	//c->SetStaticFriction (0.0f);
	//c->SetDynamicFriction(0.0f);

	//c->SetIsLocked(iw::vector3(0, 1, 0));
	//c->SetLock    (iw::vector3(0, 1, 0));

	Physics->AddRigidbody(r);

	return 0;
}

float distance; //tmp debug
iw::vector3 start;

void PlayerSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, body, player] = entity.Components.Tie<Components>();
		
		if (player->LevelTransition) {
			player->TransitionSpeed += iw::Time::DeltaTime() * 5;
		}

		if (   player->Transition
			|| player->LevelTransition)
		{
			iw::vector3& position = body->Trans().Position;

			if (player->LevelTransition) {
				position = iw::lerp(
					position,
					player->TransitionTargetPosition, 
					iw::Time::DeltaTime() * player->TransitionSpeed
				);
			}

			else {
				position = iw::lerp(
					player->TransitionStartPosition, 
					player->TransitionTargetPosition, 
					iw::Time::TotalTime() - player->Begin);
			}

			if (   iw::almost_equal(position.x, player->TransitionTargetPosition.x, 2)
				&& iw::almost_equal(position.z, player->TransitionTargetPosition.z, 2))
			{
				position = player->TransitionTargetPosition;
				player->TransitionSpeed = 1.0f;
				player->Transition = false;
				player->Timer = 0.0f;
				body->SetIsTrigger(false);
			}
		}

		else {
			if (player->DeathTimer > 0) {
				transform->Scale -= .75f / 1.0f * iw::Time::DeltaTime();
				body->SetVelocity(0.0f);

				player->DeathTimer -= iw::Time::DeltaTime();
				if (player->DeathTimer <= 0) {
					player->DeathTimer = 0;
					Bus->push<ResetLevelEvent>();
				}
			}

			else {
				player->Movement = 0;
				if (player->Left)  player->Movement.x -= 1;
				if (player->Right) player->Movement.x += 1;
				if (player->Up)    player->Movement.z -= 1;
				if (player->Down)  player->Movement.z += 1;

				player->Movement.normalize();
				player->Movement *= player->Speed;

				if (player->Timer > 0) {
					player->Movement *= 10 * player->Timer / player->DashTime;
				}

				if (player->Sprint) {
					player->Movement *= 2.0f;
				}

				//body->Trans().Position += movement * iw::Time::DeltaTime();

				body->SetVelocity(player->Movement / iw::TimeScale());

				if (player->Timer <= -player->ChargeTime) {
					if (distance == 0) {
						distance = (start - transform->Position).length();
						LOG_INFO << distance;
					}

					if (   player->Dash
						&& (player->Up || player->Down || player->Left || player->Right))
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

void PlayerSystem::OnPush() {
	if (!m_player) {
		return;
	}

	Player* player = m_player.Find<Player>();

	player->Left       = false;
	player->Right      = false;
	player->Up         = false;
	player->Down       = false;
	player->Dash       = false;
	player->Sprint     = false;
	player->Transition = false;
}

bool PlayerSystem::On(
	iw::KeyEvent& event)
{
	if (!m_player) {
		return false;
	}

	Player* player = m_player.Find<Player>();

	switch (event.Button) {
		case iw::UP: {
			player->Up = event.State;
			break;
		}
		case iw::DOWN: {
			player->Down = event.State;
			break; 
		}
		case iw::LEFT: {
			player->Left = event.State;
			break; 
		}
		case iw::RIGHT: {
			player->Right = event.State;
			break; 
		}
		case iw::X: {
			player->Dash = event.State;
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
	//Player* p = player.Find<Player>();

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
	if (   a.Has<Player>()
		&& b.Has<Bullet>())
	{
		player = a;
	}

	else if (b.Has<Player>()
		&&   a.Has<Bullet>())
	{
		player = b;
	}

	if (player.Index() != iw::EntityHandle::Empty.Index) {
		Player* p = player.Find<Player>();
		if (  true //!p->Damaged
			&& p->Health > 0)
		{
			p->Damaged = true;
			p->Health -= 1;

			float color = p->Health / 3.0f;
			m_playerModel->GetMesh(0).Material()->Set("baseColor", iw::Color(0.8f, color, color, 1));

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

			Player*        p = m_player.Find<Player>();
			iw::Rigidbody* r = m_player.Find<iw::Rigidbody>();

			p->LevelTransition = true;
			p->TransitionSpeed = 1.0f;
			p->TransitionStartPosition  = iw::vector3(r->Trans().Position.x, 1, r->Trans().Position.z);
			p->TransitionTargetPosition = iw::vector3(event.PlayerPosition.x, 1, event.PlayerPosition.y);
			p->Begin = iw::Time::TotalTime();

			r->SetCol(nullptr);

			break;
		}
		case iw::val(Actions::AT_NEXT_LEVEL): {
			Player* p = m_player.Find<Player>();

			p->LevelTransition = false;
			break;
		}
		case iw::val(Actions::START_LEVEL): {
			Player*              p = m_player.Find<Player>();
			iw::Transform*       t = m_player.Find<iw::Transform>();
			iw::Rigidbody*       r = m_player.Find<iw::Rigidbody>();
			iw::SphereCollider*  s = m_player.Find<iw::SphereCollider>();

			*p = playerPrefab;

			t->Position.x = e.as<StartLevelEvent>().PlayerPosition.x;
			t->Position.z = e.as<StartLevelEvent>().PlayerPosition.y;
			t->Position.y = 1.0f;
			t->Scale = 0.75f;
			
			r->SetCol(s);
			r->SetTrans(t);

			p->Transition = false;

			// no break
		}
		case iw::val(Actions::RESET_LEVEL): {
			m_playerModel->GetMesh(0).Material()->Set("baseColor", iw::vector4(0.8f, 1.0f));
			m_playerModel->GetMesh(0).Material()->Set("reflectance", 1.0f);
			m_playerModel->GetMesh(0).Material()->Set("refractive", 1.0f);
			break;
		}
	}

	return false;
}
