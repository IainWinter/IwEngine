#include "Systems/PLayerSystem.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"

struct Components {
	IW::Transform* Transform;
	Player*        Player;
};

PlayerSystem::PlayerSystem(
	IwEntity::Space& space,
	IW::Graphics::Renderer& renderer)
	: IwEngine::System<IW::Transform, Player>(space, renderer, "Player")
{}

void PlayerSystem::Update(
	IwEntity::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [ transform, player ] = entity.Components.Tie<Components>();

		iwm::vector3 movement;
		if (IwInput::Keyboard::KeyDown(IwInput::LEFT)) {
			movement.x -= 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::RIGHT)) {
			movement.x += 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::UP)) {
			movement.z -= 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::DOWN)) {
			movement.z += 1;
		}

		if (player->Timer <= -player->CooldownTime) {
			if (IwInput::Keyboard::KeyDown(IwInput::X)) {
				player->Timer = player->DashTime;
			}
		}

		else if (player->Timer >= -player->CooldownTime) {
			player->Timer -= IwEngine::Time::DeltaTime();
		}

		if (player->Timer > 0) {
			transform->Position += movement.normalized() * player->Speed * 10 * (player->Timer / player->DashTime) * IwEngine::Time::DeltaTime();
		}

		else {
			transform->Position += movement.normalized() * player->Speed * IwEngine::Time::DeltaTime();
		}
	}
}
