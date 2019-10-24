#include "Systems/EnemySystem.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/physics/AABB.h"

struct Components {
	IwEngine::Transform* Transform;
	Enemy*               Enemy;
};

EnemySystem::EnemySystem(
	IwEntity::Space& space,
	IwGraphics::Renderer& renderer,
	IwGraphics::ModelData* circleData)
	: IwEngine::System<IwEngine::Transform, Enemy>(space, renderer, "Enemy")
	, CircleData(circleData)
	, CircleMesh(nullptr)
{}

EnemySystem::~EnemySystem() {

}

int EnemySystem::Initialize() {
	auto& data = CircleData->Meshes[0];

	CircleMesh = new IwRenderer::Mesh();
	CircleMesh->SetVertices(data.VertexCount, data.Vertices);
	CircleMesh->SetNormals(data.VertexCount, data.Normals);
	CircleMesh->SetIndices(data.FaceCount, data.Faces);
	CircleMesh->Compile(Renderer.Device);

	return 0;
}

void EnemySystem::Destroy() {
	CircleMesh->Destroy(Renderer.Device);
}

void EnemySystem::Update(
	IwEntity::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		if (enemy->FireTime > enemy->FireTimeTotal) {
			transform->Rotation *= iwm::quaternion::create_from_euler_angles(0, 0, enemy->Speed * IwEngine::Time::DeltaTime());
		}

		else if (enemy->FireTime <= 0) {
			enemy->CanShoot = true;
			enemy->FireTime = enemy->FireTimeTotal + enemy->FireCooldown;
		}

		else if (enemy->CanShoot && enemy->FireTime <= enemy->TimeToShoot) {
			enemy->CanShoot = false;

			IwEntity::Entity spawned = Space.CreateEntity<IwEngine::Transform, IwEngine::Model, Bullet, IwPhysics::AABB2D>();
			Space.SetComponentData<IwEngine::Transform>(spawned,
				transform->Position + iwm::vector3(sqrt(2), 0, 0) * transform->Rotation,
				transform->Scale,
				transform->Rotation);
				
			Space.SetComponentData<IwEngine::Model>  (spawned, CircleData, CircleMesh, 1U);
			Space.SetComponentData<Bullet>           (spawned, LINE, 8.0f);
			Space.SetComponentData<IwPhysics::AABB2D>(spawned, iwm::vector2(-0.25f), iwm::vector2(0.25f));
		}

		enemy->FireTime -= IwEngine::Time::DeltaTime();
	}
}
