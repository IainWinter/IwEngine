#include "Systems/PLayerSystem.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"
#include <Components\Bullet.h>

PlayerSystem::PlayerSystem()
	: System<iw::Transform, iw::Rigidbody, Player>("Player")
	, dash(false)
{}

void PlayerSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [ transform, rigidbody, player ] = entity.Components.Tie<Components>();

		if (player->Timer <= -player->CooldownTime) {
			if (dash) {
				player->Timer = player->DashTime;
			}
		}

		else if (player->Timer >= -player->CooldownTime) {
			player->Timer -= iw::Time::DeltaTime();
		}

		if (player->Health <= 0) {
			Physics->RemoveRigidbody(rigidbody);
			QueueDestroyEntity(entity.Index);
		}

		if (player->Damaged) {
			LOG_INFO << player->Health;
		}

		player->Damaged = false;
	}
}

void PlayerSystem::FixedUpdate(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, rigidbody, player] = entity.Components.Tie<Components>();

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
	iw::Entity a = Space->FindEntity(event.BodyA);
	iw::Entity b = Space->FindEntity(event.BodyB);

	if (   a.Index() == iw::EntityHandle::Empty.Index
		|| b.Index() == iw::EntityHandle::Empty.Index)
	{
		return false;
	}

	iw::Entity player;
	if (a.HasComponent<Player>() && b.HasComponent<Bullet>()) {
		player = a;
	}

	else if (b.HasComponent<Player>() && a.HasComponent<Bullet>()) {
		player = b;
	}

	if (player.Index() != iw::EntityHandle::Empty.Index) {
		Player* p = player.FindComponent<Player>();
		if (!p->Damaged) {
			p->Damaged = true;
			p->Health -= 1;
		}

		return true;
	}

	return false;
}
