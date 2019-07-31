#include "GameLayer.h"
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

struct Player {
	float Speed;
	float DashSpeed;
	float DashTimeTotal;
	float DashCooldown;
	float DashTime;
};

enum EnemyType {
	SPIN
};

enum BulletType {
	LINE
};

struct Enemy {
	EnemyType Type;
	float     FireTimeTotal;
	float     TimeCooldown;
	float     TimeFrames;
};

struct Bullet {
	BulletType Type;
	float      Speed;
};

GameLayer::GameLayer()
	: IwEngine::Layer("Game")
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

	IwEntity::Entity camera = space.CreateEntity();
	space.CreateComponent<IwEngine::Transform>(camera);
	float s = .05f;
	space.CreateComponent<IwEngine::Camera>(camera, iwm::matrix4::create_orthographic(1280 * s, 720 * s, 0, 1000));

	IwEntity::Entity player = space.CreateEntity();
	space.CreateComponent<IwEngine::Transform>(player, iwm::vector3(0, 0, 1));
	space.CreateComponent<IwEngine::Model>(player, loader.Load("res/quad.obj"), device);
	space.CreateComponent<Player>(player, 10.0f, 100.0f, 0.1666f, 0.1f);

	IwEntity::Entity enemy = space.CreateEntity();
	space.CreateComponent<IwEngine::Transform>(enemy, iwm::vector3(5, 0, 1));
	space.CreateComponent<IwEngine::Model>(enemy, loader.Load("res/quad.obj"), device);
	space.CreateComponent<Enemy>(enemy, SPIN, 0.1f, 0.1f);

	return 0;
}

void GameLayer::Update() {
	for (auto c : space.ViewComponents<IwEngine::Transform, Player>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& player    = c.GetComponent<Player>();

		transform.Rotation *= iwm::quaternion::create_from_euler_angles(0, 0, IwEngine::Time::DeltaTime());

		iwm::vector3 movement;
		if (IwInput::Keyboard::KeyDown(IwInput::LEFT)) {
			movement.x += 1;
		}

		if (IwInput::Keyboard::KeyDown(IwInput::RIGHT)) {
			movement.x -= 1;
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

	for (auto c : space.ViewComponents<IwEngine::Transform, IwEngine::Camera>()) {
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

	for (auto c : space.ViewComponents<IwEngine::Transform, IwEngine::Model>()) {
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

	for (auto c : space.ViewComponents<Player>()) {
		auto& player = c.GetComponent<Player>();

		if (player.DashTime > -player.DashCooldown) {
			player.DashTime -= IwEngine::Time::DeltaTime();
		}
	}

	for (auto c : space.ViewComponents<IwEngine::Transform, Enemy>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& enemy     = c.GetComponent<Enemy>();

		if (enemy.TimeFrames < 0) {
			enemy.TimeFrames = enemy.FireTimeTotal;
			IwEntity::Entity bullet = space.CreateEntity();

			LOG_INFO << "Creating " << bullet;

			space.CreateComponent<IwEngine::Transform>(bullet, transform);
			space.CreateComponent<IwEngine::Model>(bullet, loader.Load("res/quad.obj"), device);
			space.CreateComponent<Bullet>(bullet, LINE, 10.0f);
		}

		enemy.TimeFrames -= IwEngine::Time::DeltaTime();
	}

	for (auto c : space.ViewComponents<IwEngine::Transform, Bullet>()) {
		auto& transform = c.GetComponent<IwEngine::Transform>();
		auto& bullet    = c.GetComponent<Bullet>();

		if (bullet.Type == LINE) {
			transform.Position += iwm::vector3::one * transform.Rotation * bullet.Speed * IwEngine::Time::DeltaTime();
		}

		if (transform.Position.x > 15 || transform.Position.x < -15 || transform.Position.y > 15 || transform.Position.y < -15) {
			bulletsToDestroy.push_back(c.Entity);

			LOG_INFO << "Queuing " << c.Entity;
		}
	}

	while (!bulletsToDestroy.empty()) {
		IwEntity::Entity& e = bulletsToDestroy.back();
		space.DestroyEntity(e);

		LOG_INFO << "Deleting " << e;

		bulletsToDestroy.pop_back();
	}
}

void GameLayer::FixedUpdate() {

}

void GameLayer::ImGui() {
	ImGui::Begin("Game layer");

	for (auto entity : space.ViewComponents<Player>()) {
		Player& player = entity.GetComponent<Player>();
		
		float cooldown = player.DashCooldown + player.DashTime;

		ImGui::Text("Dash frames: %f",
			player.DashTime > 0 ? player.DashTime : 0);

		ImGui::Text("Dash cooldown frames: %f",
			cooldown > 0 ? cooldown : 0);
	}

	ImGui::End();
}
