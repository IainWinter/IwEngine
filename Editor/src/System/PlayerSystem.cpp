#include "Systems/PLayerSystem.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"

PlayerSystem::PlayerSystem()
	: System<IW::Transform, IW::Rigidbody, Player>("Player")
	, dash(false)
{}

void PlayerSystem::Update(
	IW::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [ transform, rigidbody, player ] = entity.Components.Tie<Components>();

		if (player->Timer <= -player->CooldownTime) {
			if (dash) {
				player->Timer = player->DashTime;
			}
		}

		else if (player->Timer >= -player->CooldownTime) {
			player->Timer -= IW::Time::DeltaTime();
		}
	}
}

void PlayerSystem::FixedUpdate(
	IW::EntityComponentArray& view)
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
	IW::KeyEvent& event)
{
	switch (event.Button) {
		case IW::UP:    movement.z -= event.State == 0 ? -1 : 1; break;
		case IW::DOWN:  movement.z += event.State == 0 ? -1 : 1; break;
		case IW::LEFT:  movement.x -= event.State == 0 ? -1 : 1; break;
		case IW::RIGHT: movement.x += event.State == 0 ? -1 : 1; break;
		case IW::X:     dash = event.State; break;
	}

	return true;
}
