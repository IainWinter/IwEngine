#include "Systems/PlayerSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/audio/AudioSpaceStudio.h"
#include "iw/input/Devices/Keyboard.h"

PlayerSystem::PlayerSystem()
	: System<iw::Transform, iw::Rigidbody, Player>("Player")
	, m_playerModel(nullptr)
	, movement(0)
	, dash(false)
{}

int PlayerSystem::Initialize() {
	m_playerModel = Asset->Load<iw::Model>("Player");
	return 0;
}

void PlayerSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [ transform, rigidbody, player ] = entity.Components.Tie<Components>();

		if (player->DeathTimer > 0) {
			player->DeathTimer -= iw::Time::DeltaTime();

			if (player->DeathTimer <= 0) {
				player->DeathTimer = 0;
				QueueDestroyEntity(entity.Index);
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

			if (player->Damaged) {
				LOG_INFO << player->Health;
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
		case iw::UP:    movement.z -= event.State ? 1 : -1; break;
		case iw::DOWN:  movement.z += event.State ? 1 : -1; break;
		case iw::LEFT:  movement.x -= event.State ? 1 : -1; break;
		case iw::RIGHT: movement.x += event.State ? 1 : -1; break;
		case iw::X:     dash = event.State; break;
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
		case iw::val(Actions::NEXT_LEVEL ):
		case iw::val(Actions::RESET_LEVEL): m_playerModel->Meshes[0].Material->Set("albedo", iw::Color(1)); break;
	}

	return false;
}
