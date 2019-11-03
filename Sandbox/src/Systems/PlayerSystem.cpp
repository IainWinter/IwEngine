#include "Systems/PLayerSystem.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"

struct Components {
	IW::Transform* Transform;
	Player*              Player;
};

PlayerSystem::PlayerSystem(
	IwEntity::Space& space,
	IW::Graphics::Renderer& renderer)
	: IwEngine::System<IW::Transform, Player>(space, renderer, "Player")
{}

PlayerSystem::~PlayerSystem()
{

}

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

		if (player->DashTime > 0) {
			transform->Position += movement * player->DashSpeed * player->DashTime / player->DashTimeTotal * IwEngine::Time::DeltaTime();
			if (IwInput::Keyboard::KeyUp(IwInput::X)) {
				player->DashTime = 0;
			}
		}

		else {
			if (player->DashTime + player->DashCooldown <= 0 && IwInput::Keyboard::KeyDown(IwInput::X)) {
				player->DashTime = player->DashTimeTotal;
			}

			else {
				transform->Position += movement * player->Speed * IwEngine::Time::DeltaTime();
			}
		}

		if (player->DashTime > -player->DashCooldown) {
			player->DashTime -= IwEngine::Time::DeltaTime();
		}
	}
}
