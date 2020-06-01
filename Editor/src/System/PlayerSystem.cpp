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
	, up(false)
	, down(false)
	, left(false)
	, right(false)
	, dash(false)
	, sprint(false)
	, transition(false)
	, levelTransition(false)
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
		playerPrefab.Speed        = 4.25f;
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

	player = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Player>();
	
	                         player.Set<iw::Model>(*m_playerModel);
	                         player.Set<Player>(playerPrefab);
	iw::Transform*       t = player.Set<iw::Transform>(iw::vector3(0, 1, 0), iw::vector3(1));
	iw::SphereCollider*  s = player.Set<iw::SphereCollider>(iw::vector3::zero, 0.75f);
	iw::Rigidbody*       r = player.Set<iw::Rigidbody>();

	//c->SetMass(1);
	r->SetCol(s);
	r->SetTrans(t);
	r->SetIsStatic(false);

	r->SetOnCollision([&](iw::Manifold& man, float dt) {
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

		if (playerComponent->Timer <= 0) {
			return;
		}

		if (enemyComponent->Type >= EnemyType::MINI_BOSS_BOX_SPIN) {
			if (transition) return;

			Player*        playerComp  = player.Find<Player>();
			iw::Transform* playerTrans = player.Find<iw::Transform>();
			iw::Rigidbody* playerBody  = player.Find<iw::Rigidbody>();

			playerComp->Timer = 0.0f;
			playerBody->SetIsTrigger(true);

			transitionSpeed = 1.5f;
			transition = true;
			transitionStartPosition  = playerTrans->Position;
			transitionTargetPosition = playerTrans->Position + movement.normalized() * 12;
			begin = iw::Time::TotalTime();
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
		
		if (levelTransition) {
			transitionSpeed += 0.01f;
		}

		if (transition || levelTransition) {
			if (levelTransition) {
				body->Trans().Position = iw::lerp(body->Trans().Position, transitionTargetPosition, iw::Time::DeltaTime() * transitionSpeed);
			}

			else {
				body->Trans().Position = iw::lerp(transitionStartPosition, transitionTargetPosition, iw::Time::TotalTime() - begin);
			}

			if (   iw::almost_equal(body->Trans().Position.x, transitionTargetPosition.x, 2)
				&& iw::almost_equal(body->Trans().Position.z, transitionTargetPosition.z, 2))
			{
				transitionSpeed = 1.0f;
				transition = false;
				body->Trans().Position = transitionTargetPosition;
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
				movement = 0;
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

				//body->Trans().Position += movement * iw::Time::DeltaTime();

				body->SetVelocity(movement / iw::TimeScale());

				if (player->Timer <= -player->ChargeTime) {
					if (distance == 0) {
						distance = (start - transform->Position).length();
						LOG_INFO << distance;
					}

					if (   dash
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

void PlayerSystem::OnPush() {
	left = right = up = down = dash = sprint = transition = false;
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
		if (  !p->Damaged
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

			Player*        p = player.Find<Player>();
			iw::Rigidbody* r = player.Find<iw::Rigidbody>();

			r->SetCol(nullptr);

			transitionSpeed = 1.0f;
			levelTransition = true;
			transitionStartPosition  = iw::vector3(r->Trans().Position.x, 1, r->Trans().Position.z);
			transitionTargetPosition = iw::vector3(event.PlayerPosition.x, 1, event.PlayerPosition.y);
			begin = iw::Time::TotalTime();

			//t->Scale = 0;

			//up = down = left = right = dash = sprint = false;

			break;
		}
		case iw::val(Actions::AT_NEXT_LEVEL): {
			levelTransition = false;
			break;
		}
		case iw::val(Actions::START_LEVEL): {
			Player*              p = player.Find<Player>();
			iw::Transform*       t = player.Find<iw::Transform>();
			iw::Rigidbody*       r = player.Find<iw::Rigidbody>();
			iw::SphereCollider*  s = player.Find<iw::SphereCollider>();

			*p = playerPrefab;

			t->Position.x = e.as<StartLevelEvent>().PlayerPosition.x;
			t->Position.z = e.as<StartLevelEvent>().PlayerPosition.y;
			t->Position.y = 1.0f;
			t->Scale = 0.75f;
			
			r->SetCol(s);
			r->SetTrans(t);
			r->SetSimGravity(false);

			transition = false; // put in player

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
