#include "Systems/PLayerSystem.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"

namespace IW {
	PlayerSystem::PlayerSystem()
		: System<Transform, Rigidbody, Player>("Player")
		, dash(false)
	{}

	void PlayerSystem::Update(
		EntityComponentArray& view)
	{
		for (auto entity : view) {
			auto [ transform, rigidbody, player ] = entity.Components.Tie<Components>();

			if (player->Timer <= -player->CooldownTime) {
				if (dash) {
					player->Timer = player->DashTime;
				}
			}

			else if (player->Timer >= -player->CooldownTime) {
				player->Timer -= Time::DeltaTime();
			}

			if (movement != 0) {
				movement.normalize();
				if (player->Timer > 0) {
					transform->Position += movement * player->Speed * 10 * (player->Timer / player->DashTime) * Time::DeltaTime();
				}

				else {
					transform->Position += movement * player->Speed * Time::DeltaTime();
				}
			}
		}
	}

	bool PlayerSystem::On(
		KeyEvent& event)
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
}
