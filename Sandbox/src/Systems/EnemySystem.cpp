#include "Systems/EnemySystem.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include <iw\physics\AABB.h>

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

int max = 1;

void EnemySystem::Update(
	View& view)
{
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
			Space.CreateComponent<IwEngine::Model>(bullet, CircleData, CircleMesh, 1U);
			Space.CreateComponent<IwPhysics::AABB3D>(bullet, iwm::vector3::zero, 1.0f);
			Space.CreateComponent<Bullet>(bullet, LINE, 4.0f);
		}

		enemy.FireTime -= IwEngine::Time::DeltaTime();
	}
}
