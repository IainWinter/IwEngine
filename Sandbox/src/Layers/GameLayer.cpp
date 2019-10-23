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

#include "iw/renderer/Mesh.h"

GameLayer::GameLayer(
	IwEntity::Space& space,
	IwGraphics::Renderer& renderer)
	: IwEngine::Layer(space, renderer, "Game")
	, pipeline(nullptr)
	, QuadMesh(nullptr)
	, QuadData(nullptr)
{
	IwGraphics::ModelData* circle = loader.Load("res/circle.obj");
	QuadData = loader.Load("res/quad.obj");

	PushSystem<BulletSystem>();
	PushSystem<EnemySystem>(circle);
	PushSystem<PlayerSystem>();
	PushSystem<IwEngine::PhysicsSystem>();
}

GameLayer::~GameLayer() {

}

struct CameraComponents {
	IwEngine::Transform* Transform;
	IwEngine::Camera* Camera;
};

struct PlayerComponents {
	Player* Players;
};

struct ModelComponents {
	IwEngine::Transform* Transform;
	IwEngine::Model* Model;
};

int GameLayer::Initialize(
	IwEngine::InitOptions& options)
{
	// Making line mesh
	{
		// Model data
		verts = new iwm::vector3[4] {
			iwm::vector3(.25f, 0, 0), iwm::vector3(1, 1, 0),
			iwm::vector3(4, 0, 0), iwm::vector3(1, 0, 1)
		};

		unsigned int index[2] = {
			0, 1
		};

		IwRenderer::VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);

		// non async things

		IwRenderer::IIndexBuffer* ibuffer = Renderer.Device->CreateIndexBuffer(2, index);
		IwRenderer::IVertexArray* varray  = Renderer.Device->CreateVertexArray();

		IwRenderer::IVertexBuffer* vbuffer = Renderer.Device->CreateVertexBuffer(4 * sizeof(iwm::vector3), verts);
		varray->AddBuffer(vbuffer, layout);


		LineMesh = new IwGraphics::Mesh { varray, ibuffer, 2 };
	}

	// Making quad mesh
	{
		QuadMesh = Renderer.CreateMesh(QuadData->Meshes[0]);
	}
	
	// Shader program

	{
		auto vs = Renderer.Device->CreateVertexShader(iwu::ReadFile("res/sandboxvs.glsl").c_str());
		auto fs = Renderer.Device->CreateFragmentShader(iwu::ReadFile("res/sandboxfs.glsl").c_str());

		pipeline = Renderer.Device->CreatePipeline(vs, fs);

		auto vs2 = Renderer.Device->CreateVertexShader(iwu::ReadFile("res/sandbox_line_vs.glsl").c_str());
		auto fs2 = Renderer.Device->CreateFragmentShader(iwu::ReadFile("res/sandbox_line_fs.glsl").c_str());

		pipeline_line = Renderer.Device->CreatePipeline(vs2, fs2);
	}

	IwEntity::Entity camera = Space.CreateEntity<IwEngine::Transform, IwEngine::Camera>();
	Space.SetComponentData<IwEngine::Transform>(camera, iwm::vector3::zero, iwm::vector3::one, iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0));
	Space.SetComponentData<IwEngine::Camera>   (camera, iwm::matrix4::create_orthographic(64, 36, -1, 1));
	//Space.SetComponentData<IwEngine::Camera>   (camera, iwm::matrix4::create_perspective_field_of_view(1.17f, 1.77f, 0.001f, 1000.0f));

	IwEntity::Entity player = Space.CreateEntity<IwEngine::Transform, IwEngine::Model, Player, IwPhysics::AABB2D>();
	Space.SetComponentData<IwEngine::Transform>(player, iwm::vector3(10, 0, 0));
	Space.SetComponentData<IwEngine::Model>    (player, QuadData, QuadMesh, 1U);
	Space.SetComponentData<Player>             (player, 10.0f, 100.0f, 0.1666f, 0.1f);
	Space.SetComponentData<IwPhysics::AABB2D>  (player, iwm::vector2(-1), iwm::vector2(1));

	for (float x = 5; x < 6; x++) {
		for (float y = 5; y < 6; y++) {
			IwEntity::Entity enemy = Space.CreateEntity<IwEngine::Transform, IwEngine::Model, Enemy, IwPhysics::AABB2D>();
			Space.SetComponentData<IwEngine::Transform>(enemy, iwm::vector3(x * 3 - 15, y * 3 - 15, 0));
			Space.SetComponentData<IwEngine::Model>    (enemy, QuadData, QuadMesh, 1U);
			Space.SetComponentData<Enemy>              (enemy, SPIN, 3.0f, 0.05f, 0.025f, 0.025f);
			Space.SetComponentData<IwPhysics::AABB2D>  (enemy, iwm::vector2(-1), iwm::vector2(1));
		}
	}

	return Layer::Initialize(options);
}

void GameLayer::PostUpdate() {
	verts[0].x += .1f;

	Renderer.Device->SetPipeline(pipeline);

	for (auto c : Space.Query<IwEngine::Transform, IwEngine::Camera>()) {
		auto [transform, camera] = c.Components.Tie<CameraComponents>();

		pipeline->GetParam("proj")
			->SetAsMat4(camera->Projection);

		pipeline->GetParam("view")
			->SetAsMat4(iwm::matrix4::create_look_at(
				transform->Position,
				transform->Position - transform->Forward(),
				transform->Up()));
	}

	for (auto entity : Space.Query<IwEngine::Transform, IwEngine::Model>()) {
		auto [transform, model] = entity.Components.Tie<ModelComponents>();

		pipeline->GetParam("model")
			->SetAsMat4(transform->Transformation());

		for (int i = 0; i < model->MeshCount; i++) {
			IwGraphics::Mesh& mesh = model->Meshes[i];
			if (mesh.IndexBuffer.initialized() && mesh.VertexArray.initialized()) {
				Renderer.Device->SetVertexArray(mesh.VertexArray.value());
				Renderer.Device->SetIndexBuffer(mesh.IndexBuffer.value());
				Renderer.Device->DrawElements(IwRenderer::TRIANGLES, mesh.ElementCount, 0);
			}
		}
	}

	Renderer.Device->SetPipeline(pipeline_line);

	for (auto c : Space.Query<IwEngine::Transform, IwEngine::Camera>()) {
		auto [transform, camera] = c.Components.Tie<CameraComponents>();

		pipeline->GetParam("proj")
			->SetAsMat4(camera->Projection);

		pipeline->GetParam("view")
			->SetAsMat4(iwm::matrix4::create_look_at(
				transform->Position,
				transform->Position - transform->Forward(),
				transform->Up()));
	}

	for (auto entity : Space.Query<IwEngine::Transform, Bullet>()) {
		auto [transform] = entity.Components.TieTo<IwEngine::Transform>();

		pipeline->GetParam("model")
			->SetAsMat4(transform->Transformation());

		Renderer.Device->SetVertexArray(LineMesh->VertexArray.value());
		Renderer.Device->SetIndexBuffer(LineMesh->IndexBuffer.value());
		Renderer.Device->DrawElements(IwRenderer::LINES, LineMesh->ElementCount, 0);
	}
}

void GameLayer::FixedUpdate() {

}

void GameLayer::ImGui() {
	ImGui::Begin("Game layer");

	ImGui::Text("sdfsdf");

	for (auto entity : Space.Query<Player>()) {
		auto [player] = entity.Components.TieTo<Player>();
		
		float cooldown = player->DashCooldown + player->DashTime;

		ImGui::Text("Dash time: %f",
			player->DashTime > 0 ? player->DashTime : 0);

		ImGui::Text("Dash cooldown time: %f",
			cooldown > 0 ? cooldown : 0);
	}

	int i = 0;
	for (auto entity : Space.Query<Bullet>()) {
		++i;
	}

	ImGui::Text("Bullet count: %i", i);

	i = 0;
	for (auto entity : Space.Query<Enemy>()) {
		++i;
	}

	ImGui::Text("Enemy count: %i", i);


	ImGui::End();
}
