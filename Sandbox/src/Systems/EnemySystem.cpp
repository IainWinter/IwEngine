#include "Systems/EnemySystem.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include <iw\physics\AABB.h>

struct Components {
	IwEngine::Transform* Transform;
	Enemy*               Enemy;
};

EnemySystem::EnemySystem(
	IwEntity::Space& space,
	IwGraphics::RenderQueue& renderQueue,
	IwGraphics::ModelData* circleData)
	: IwEngine::System<IwEngine::Transform, Enemy>(space, renderQueue, "Enemy")
	, CircleData(circleData)
	, CircleMesh(nullptr)
{}

EnemySystem::~EnemySystem() {

}

int EnemySystem::Initialize() {
	IwGraphics::MeshData& meshData = CircleData->Meshes[0];

	IwRenderer::VertexBufferLayout layouts[1];
	layouts[0].Push<float>(3);
	layouts[0].Push<float>(3);

	iwu::potential<IwRenderer::IVertexBuffer*> buffers[1];
	buffers[0] = RenderQueue.QueuedDevice.CreateVertexBuffer(meshData.VertexCount * sizeof(IwGraphics::Vertex), meshData.Vertices);

	auto pib = RenderQueue.QueuedDevice.CreateIndexBuffer(meshData.FaceCount, meshData.Faces);
	auto pva = RenderQueue.QueuedDevice.CreateVertexArray(1, buffers, layouts);

	CircleMesh = new IwGraphics::Mesh{ pva, pib, meshData.FaceCount };

	return 0;
}

void EnemySystem::Destroy() {
	RenderQueue.QueuedDevice.DestroyVertexArray(CircleMesh->VertexArray);
	RenderQueue.QueuedDevice.DestroyIndexBuffer(CircleMesh->IndexBuffer);
	delete CircleMesh;
}

void EnemySystem::Update(
	View& view)
{
	for (auto entity : view) {
		auto [transform, enemy ] = entity->Components->Tie<Components>();

		if (enemy->FireTime > enemy->FireTimeTotal) {
			transform->Rotation *= iwm::quaternion::create_from_euler_angles(0, 0, enemy->Speed * IwEngine::Time::DeltaTime());
		}

		else if (enemy->FireTime <= 0) {
			enemy->CanShoot = true;
			enemy->FireTime = enemy->FireTimeTotal + enemy->FireCooldown;
		}

		else if (enemy->CanShoot && enemy->FireTime <= enemy->TimeToShoot) {
			enemy->CanShoot = false;
			iwu::ref<IwEntity::Entity2> spawned = Space.CreateEntity<IwEngine::Transform, IwEngine::Model, Bullet>();

			auto [transform, model, bullet] = spawned->Components->Tie<IwEngine::Transform, IwEngine::Model, Bullet>();
			transform = IwEngine::Transform { transform.Position + iwm::vector3(1, 1, 0) * transform.Rotation.inverted(), transform.Scale, transform.Rotation.inverted() };
			model     = IwEngine::Model { CircleData, CircleMesh, 1U };
			bullet    = Bullet { LINE, 4.0f };
		}

		enemy->FireTime -= IwEngine::Time::DeltaTime();
	}
}
