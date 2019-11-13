#include "Systems/EnemySystem.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/physics/AABB.h"

struct Components {
	IW::Transform* Transform;
	Enemy*         Enemy;
};

EnemySystem::EnemySystem(
	IwEntity::Space& space,
	IW::Graphics::Renderer& renderer,
	IW::Mesh* circle)
	: IwEngine::System<IW::Transform, Enemy>(space, renderer, "Enemy")
	, CircleMesh(circle)
{}

void EnemySystem::Destroy() {
	CircleMesh->Destroy(Renderer.Device);
}

void EnemySystem::Update(
	IwEntity::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		if (  !enemy->HasShot
			&& enemy->Timer <= 0)
		{
			enemy->HasShot = true;

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iwm::PI2);
			iwm::quaternion rot = iwm::quaternion::from_euler_angles(0, enemy->Rotation, 0);

			IwEntity::Entity spawned = Space.CreateEntity<IW::Transform, IwEngine::Model, Bullet, IwPhysics::AABB2D>();
			Space.SetComponentData<IW::Transform>(spawned,
				transform->Position + iwm::vector3(sqrt(2), 0, 0) * rot,
				iwm::vector3(.25f),
				rot);

			Space.SetComponentData<IwEngine::Model>(spawned, CircleMesh, 1U);
			Space.SetComponentData<Bullet>(spawned, LINE, 5.0f);
			Space.SetComponentData<IwPhysics::AABB2D>(spawned, iwm::vector2(-0.25f), iwm::vector2(0.25f));
		}

		if (enemy->Timer <= -enemy->CooldownTime) {
			enemy->HasShot = false;
			enemy->Timer = enemy->FireTime;
		}

		if (enemy->Timer >= -enemy->CooldownTime) {
			enemy->Timer -= IwEngine::Time::DeltaTime();
		}
	}
}
