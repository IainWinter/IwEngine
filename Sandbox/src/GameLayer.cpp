#include "GameLayer.h"
#include "Components/Player.h"
#include "Components/Enemy.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Transform.h"
#include "iw/engine/Components/Model.h"
#include "iw/engine/Components/Camera.h"
#include "iw/engine/Components/Destroy.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/util/io/File.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/input/Devices/Mouse.h"
#include "imgui/imgui.h"

GameLayer::GameLayer(
	IwEntity::Space& space)
	: IwEngine::Layer(space, "Game")
	, device(nullptr)
	, pipeline(nullptr)
{}

GameLayer::~GameLayer() {
	device->DestroyPipeline(pipeline);
	delete device;
}

int GameLayer::Initialize(
	IwEngine::InitOptions& options)
{
	device = new IwRenderer::GLDevice();

	auto vs = device->CreateVertexShader(iwu::ReadFile("res/sandboxvs.glsl").c_str());
	auto fs = device->CreateFragmentShader(iwu::ReadFile("res/sandboxfs.glsl").c_str());

	pipeline = device->CreatePipeline(vs, fs);
	device->SetPipeline(pipeline);

	IwEntity::Entity camera = Space.CreateEntity();
	Space.CreateComponent<IwEngine::Transform>(camera, iwm::vector3::zero, iwm::vector3::one, iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0));
	float s = .05f;
	Space.CreateComponent<IwEngine::Camera>(camera, iwm::matrix4::create_orthographic(1280 * s, 720 * s, 0, -1000)); //camera has flipped x axis

	IwEntity::Entity player = Space.CreateEntity();
	Space.CreateComponent<IwEngine::Transform>(player, iwm::vector3(0, 0, 1));
	Space.CreateComponent<IwEngine::Model>(player, loader.Load("res/quad.obj"), device);
	Space.CreateComponent<Player>(player, 10.0f, 100.0f, 0.1666f, 0.1f);

	for (size_t x = 0; x < 5; x++) {
		for (size_t y = 0; y < 5; y++) {
			IwEntity::Entity e = Space.CreateEntity();
			Space.CreateComponent<IwEngine::Transform>(e, iwm::vector3(x * 3, y * 3, 1));
			Space.CreateComponent<IwEngine::Model>(e, loader.Load("res/quad.obj"), device);
			Space.CreateComponent<Enemy>(e, SPIN, 3.0f, 0.05f, 0.025f, 0.025f);
		}
	}

	return 0;
}

void GameLayer::Update() {
	for (auto c : Space.ViewComponents<IwEngine::Transform, Player>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& player    = c.GetComponent<Player>();

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
	}

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
			device->SetVertexArray(model.Meshes[i].VertexArray);
			device->SetIndexBuffer(model.Meshes[i].IndexBuffer);
			device->DrawElements(model.Meshes[i].FaceCount, 0);
		}
	}

	for (auto c : Space.ViewComponents<Player>()) {
		auto& player = c.GetComponent<Player>();

		if (player.DashTime > -player.DashCooldown) {
			player.DashTime -= IwEngine::Time::DeltaTime();
		}
	}

	for (auto c : Space.ViewComponents<IwEngine::Transform, Enemy>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& enemy     = c.GetComponent<Enemy>();

		if (enemy.FireTime > enemy.FireTimeTotal) {
			transform.Rotation *= iwm::quaternion::create_from_euler_angles(0, 0, enemy.Speed * IwEngine::Time::DeltaTime());
		}

		else if (enemy.FireTime <= 0) {
			enemy.CanShoot    = true;
			enemy.FireTime = enemy.FireTimeTotal + enemy.FireCooldown;
		}

		else if (enemy.CanShoot && enemy.FireTime <= enemy.TimeToShoot) {
			enemy.CanShoot = false;
			IwEntity::Entity bullet = Space.CreateEntity();
			Space.CreateComponent<IwEngine::Transform>(bullet, transform.Position + iwm::vector3(1, 1, 0) * transform.Rotation.inverted(), transform.Scale, transform.Rotation.inverted());
			Space.CreateComponent<IwEngine::Model>(bullet, loader.Load("res/circle.obj"), device);
			Space.CreateComponent<Bullet>(bullet, LINE, 4.0f);
		}

		enemy.FireTime -= IwEngine::Time::DeltaTime();
	}
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
