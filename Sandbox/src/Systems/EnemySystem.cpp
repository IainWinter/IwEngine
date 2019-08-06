#include "Systems/EnemySystem.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/input/Devices/Keyboard.h"

EnemySystem::EnemySystem(
	IwEntity::Space& space,
	IwGraphics::RenderQueue& renderQueue,
	IwGraphics::ModelData* circle)
	: IwEngine::System<IwEngine::Transform, Enemy>(space, renderQueue, "Enemy")
	, Circle(circle)
{}

EnemySystem::~EnemySystem()
{

}

void EnemySystem::Update(
	View& view)
{
	if (IwInput::Keyboard::KeyDown(IwInput::SPACE)) {
		for (auto components : view) {
			auto& transform = components.GetComponent<IwEngine::Transform>();
			auto& enemy = components.GetComponent<Enemy>();

			if (enemy.FireTime > enemy.FireTimeTotal) {
				transform.Rotation *= iwm::quaternion::create_from_euler_angles(0, 0, enemy.Speed * IwEngine::Time::DeltaTime());
			}

			else if (enemy.FireTime <= 0) {
				enemy.CanShoot = true;
				enemy.FireTime = enemy.FireTimeTotal + enemy.FireCooldown;
			}

			else if (enemy.CanShoot && enemy.FireTime <= enemy.TimeToShoot) {
				enemy.CanShoot = false;
				IwEntity::Entity bullet = Space.CreateEntity();
				Space.CreateComponent<IwEngine::Transform>(bullet, transform.Position + iwm::vector3(1, 1, 0) * transform.Rotation.inverted(), transform.Scale, transform.Rotation.inverted());
				Space.CreateComponent<IwEngine::Model>(bullet, Circle, RenderQueue);
				Space.CreateComponent<Bullet>(bullet, LINE, 4.0f);
			}

			enemy.FireTime -= IwEngine::Time::DeltaTime();
		}
	}
}
