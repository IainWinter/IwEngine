#include "Layers/GameLayer.h"
#include "Systems/BulletSystem.h"
#include "Systems/EnemySystem.h"
#include "Systems/PlayerSystem.h"
#include "Components/Player.h"
#include "Components/Enemy.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Transform.h"
#include "iw/engine/Components/Model.h"
#include "iw/engine/Components/Camera.h"
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

	PushSystem<BulletSystem>();
	PushSystem<EnemySystem>(circle);
	PushSystem<PlayerSystem>();
}

GameLayer::~GameLayer() {

}

int GameLayer::Initialize(
	IwEngine::InitOptions& options)
{
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
	Space.CreateComponent<IwEngine::Model>(player, loader.Load("res/quad.obj"), RenderQueue);
	Space.CreateComponent<Player>(player, 10.0f, 100.0f, 0.1666f, 0.1f);

	for (size_t x = 0; x < 2; x++) {
		for (size_t y = 0; y < 2; y++) {
			IwEntity::Entity e = Space.CreateEntity();
			Space.CreateComponent<IwEngine::Transform>(e, iwm::vector3(x * 3, y * 3, 1));
			Space.CreateComponent<IwEngine::Model>(e, loader.Load("res/quad.obj"), RenderQueue);
			Space.CreateComponent<Enemy>(e, SPIN, 3.0f, 0.05f, 0.025f, 0.025f);
		}
	}

	return 0;
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
