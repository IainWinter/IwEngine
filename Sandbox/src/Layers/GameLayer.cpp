#include "Layers/GameLayer.h"
#include "Systems/BulletSystem.h"
#include "Systems/EnemySystem.h"
#include "Systems/PlayerSystem.h"
#include "Components/Player.h"
#include "Components/Enemy.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/data/Components/Transform.h"
#include "iw/engine/Components/Model.h"
#include "iw/engine/Components/Camera.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/util/io/File.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/input/Devices/Mouse.h"
#include "imgui/imgui.h"

GameLayer::GameLayer(
	IwEntity::Space& space,
	IwGraphics::RenderQueue& renderQueue)
	: IwEngine::Layer(space, renderQueue, "Game")
	, pipeline(nullptr)
{
	IwGraphics::ModelData* circle = loader.Load("res/circle.obj");
	QuadData = loader.Load("res/quad.obj");

	PushSystem<BulletSystem>();
	PushSystem<EnemySystem>(circle);
	PushSystem<PlayerSystem>();
	PushSystem<PlayerSystem>();
	PushSystem<IwEngine::PhysicsSystem>();
}

GameLayer::~GameLayer() {

}

int GameLayer::Initialize(
	IwEngine::InitOptions& options)
{
	IwGraphics::MeshData& meshData = QuadData->Meshes[0];

	IwRenderer::VertexBufferLayout layouts[1];
	layouts[0].Push<float>(3);
	layouts[0].Push<float>(3);

	iwu::potential<IwRenderer::IVertexBuffer*> buffers[1];
	buffers[0] = RenderQueue.QueuedDevice.CreateVertexBuffer(meshData.VertexCount * sizeof(IwGraphics::Vertex), meshData.Vertices);

	auto pib = RenderQueue.QueuedDevice.CreateIndexBuffer(meshData.FaceCount, meshData.Faces);
	auto pva = RenderQueue.QueuedDevice.CreateVertexArray(1, buffers, layouts);

	QuadMesh = new IwGraphics::Mesh{ pva, pib, meshData.FaceCount };

	auto vs = RenderQueue.QueuedDevice.Device.CreateVertexShader(iwu::ReadFile("res/sandboxvs.glsl").c_str());
	auto fs = RenderQueue.QueuedDevice.Device.CreateFragmentShader(iwu::ReadFile("res/sandboxfs.glsl").c_str());

	pipeline = RenderQueue.QueuedDevice.Device.CreatePipeline(vs, fs);
	RenderQueue.QueuedDevice.Device.SetPipeline(pipeline);

	IwEntity::Entity camera = Space.CreateEntity();
	Space.CreateComponent<IwEngine::Transform>(camera, iwm::vector3::zero, iwm::vector3::one, iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0));
	float s = .05f;
	Space.CreateComponent<IwEngine::Camera>(camera, iwm::matrix4::create_orthographic(1280 * s, 720 * s, 0, -1000)); //camera has flipped x axis

	IwEntity::Entity player = Space.CreateEntity();
	Space.CreateComponent<IwEngine::Transform>(player, iwm::vector3(0, 0, 1));
	Space.CreateComponent<IwEngine::Model>(player, QuadData, QuadMesh, 1U);
	Space.CreateComponent<Player>(player, 10.0f, 100.0f, 0.1666f, 0.1f);
	Space.CreateComponent<IwPhysics::AABB3D>(player, iwm::vector3::zero, 1.0f);

	IwEntity::Entity enemy = Space.CreateEntity();
	Space.CreateComponent<IwEngine::Transform>(enemy, iwm::vector3(3.5f, 0, 1));
	Space.CreateComponent<IwEngine::Model>(enemy, QuadData, QuadMesh, 1U);
	Space.CreateComponent<Enemy>(enemy, SPIN, 3.0f, 0.05f, 0.025f, 0.025f);
	Space.CreateComponent<IwPhysics::AABB3D>(enemy, iwm::vector3::zero, 1.0f);

	return Layer::Initialize(options);
}

void GameLayer::Update() {
	UpdateSystems();

	for (auto c : Space.ViewComponents<IwEngine::Transform, IwEngine::Camera>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& camera    = c.GetComponent<IwEngine::Camera>();

		pipeline->GetParam("proj")
			->SetAsMat4(camera.Projection);

		pipeline->GetParam("view")
			->SetAsMat4(iwm::matrix4::create_look_at(
				transform.Position,
				transform.Position - transform.Forward(),
				transform.Up()));
	}

	for (auto c : Space.ViewComponents<IwEngine::Transform, IwEngine::Model>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& model     = c.GetComponent<IwEngine::Model>();

		pipeline->GetParam("model")
			->SetAsMat4(transform.Transformation());

		for (int i = 0; i < model.MeshCount; i++) {
			IwGraphics::Mesh& mesh = model.Meshes[i];
			if (mesh.IndexBuffer.initialized() && mesh.VertexArray.initialized()) {
				RenderQueue.QueuedDevice.Device.SetVertexArray(mesh.VertexArray.value());
				RenderQueue.QueuedDevice.Device.SetIndexBuffer(mesh.IndexBuffer.value());
				RenderQueue.QueuedDevice.Device.DrawElements(mesh.FaceCount, 0);
			}
		}
	}

	RenderQueue.Execute();
}

void GameLayer::FixedUpdate() {

}

void GameLayer::ImGui() {
	ImGui::Begin("Game layer");

	for (auto entity : Space.ViewComponents<Player>()) {
		Player& player = entity.GetComponent<Player>();
		
		float cooldown = player.DashCooldown + player.DashTime;

		ImGui::Text("Dash frames: %f",
			player.DashTime > 0 ? player.DashTime : 0);

		ImGui::Text("Dash cooldown frames: %f",
			cooldown > 0 ? cooldown : 0);
	}

	ImGui::End();
}
