#include "Systems/PLayerSystem.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"

struct Components {
	IW::Transform* Transform;
	Player*        Player;
};

PlayerSystem::PlayerSystem(
	IW::Space& space,
	IW::Graphics::Renderer& renderer)
	: IW::System<IW::Transform, Player>(space, renderer, "Player")
{}

void PlayerSystem::Update(
	IW::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [ transform, player ] = entity.Components.Tie<Components>();

		iw::vector3 movement;
		if (IW::Keyboard::KeyDown(IW::LEFT)) {
			movement.x -= 1;
		}

		if (IW::Keyboard::KeyDown(IW::RIGHT)) {
			movement.x += 1;
		}

		if (IW::Keyboard::KeyDown(IW::UP)) {
			movement.z -= 1;
		}

		if (IW::Keyboard::KeyDown(IW::DOWN)) {
			movement.z += 1;
		}

		if (player->Timer <= -player->CooldownTime) {
			if (IW::Keyboard::KeyDown(IW::X)) {
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
