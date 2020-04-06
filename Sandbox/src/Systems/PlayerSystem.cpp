#include "Systems/PLayerSystem.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"

struct Components {
	IW::Transform* Transform;
	Player*        Player;
};

PlayerSystem::PlayerSystem()
	: IW::System<IW::Transform, Player>("Player")
	, dash(false)
{}

void PlayerSystem::Update(
	IW::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [ transform, player ] = entity.Components.Tie<Components>();

		if (player->Timer <= -player->CooldownTime) {
			if (dash) {
				player->Timer = player->DashTime;
			}
		}

		else if (player->Timer >= -player->CooldownTime) {
			player->Timer -= IW::Time::DeltaTime();
		}

		if (player->Timer > 0) {
			transform->Position += movement.normalized() * player->Speed * 10 * (player->Timer / player->DashTime) * IW::Time::DeltaTime();
		}

		else {
			transform->Position += movement.normalized() * player->Speed * IW::Time::DeltaTime();
		}
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
		case IW::X:     dash   = event.State; break;
		case iw::C:     sprint = event.State; break;
	}

	return true;
}
