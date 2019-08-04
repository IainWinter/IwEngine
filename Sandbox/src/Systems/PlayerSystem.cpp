#include "Systems/PLayerSystem.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"

PlayerSystem::PlayerSystem(
	IwEntity::Space& space)
	: IwEngine::System<IwEngine::Transform, Player>(space, "Player")
{}

PlayerSystem::~PlayerSystem()
{

}

void PlayerSystem::Update(
	View& view)
{
	for (auto components : view) {
		auto& transform = components.GetComponent<IwEngine::Transform>();
		auto& player    = components.GetComponent<Player>();

		transform.Rotation *= iwm::quaternion::create_from_euler_angles(0, 0, IwEngine::Time::DeltaTime());

		iwm::vector3 movement;
		if (IwInput::Keyboard::KeyDown(IwInput::LEFT)) {
			movement.x -= 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::RIGHT)) {
			movement.x += 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::UP)) {
			movement.y += 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::DOWN)) {
			movement.y -= 1;
		}

		if (player.DashTime > 0) {
			transform.Position += movement * player.DashSpeed * player.DashTime / player.DashTimeTotal * IwEngine::Time::DeltaTime();
			if (IwInput::Keyboard::KeyUp(IwInput::X)) {
				player.DashTime = 0;
			}
		}

		else {
			if (player.DashTime + player.DashCooldown <= 0 && IwInput::Keyboard::KeyDown(IwInput::X)) {
				player.DashTime = player.DashTimeTotal;
			}

			else {
				transform.Position += movement * player.Speed * IwEngine::Time::DeltaTime();
			}
		}

		if (player.DashTime > -player.DashCooldown) {
			player.DashTime -= IwEngine::Time::DeltaTime();
		}
	}
}
