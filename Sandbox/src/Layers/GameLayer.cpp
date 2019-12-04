//#include "Layers/GameLayer.h"
//#include "Systems/BulletSystem.h"
//#include "Systems/EnemySystem.h"
//#include "Systems/PlayerSystem.h"
//#include "Components/Player.h"
//#include "Components/Enemy.h"
//#include "Components/Bullet.h"
//#include "iw/engine/Time.h"
//#include "iw/common/Components/Transform.h"
//#include "iw/engine/Components/Model.h"
//#include "iw/engine/Components/CameraController.h"
//#include "iw/engine/Systems/PhysicsSystem.h"
//#include "iw/renderer/Platform/OpenGL/GLDevice.h"
//#include "iw/util/io/File.h"
//#include "iw/input/Devices/Keyboard.h"
//#include "iw/input/Devices/Mouse.h"
//#include "imgui/imgui.h"
//
//#include "iw/graphics/Mesh.h"
//
//#include "iw/graphics/Camera.h"
//
//GameLayer::GameLayer(
//	IW::Space& space,
//	IW::Renderer& renderer,
//	IW::AssetManager& asset)
//	: IW::Layer(space, renderer, asset, "Game")
//	, line(nullptr)
//{
//	PushSystem<BulletSystem>();
//	PushSystem<PlayerSystem>();
//	PushSystem<IW::PhysicsSystem>();
//}
//
//struct CameraComponents {
//	IW::Transform* Transform;
//	IW::OrthographicCamera* Camera;
//};
//
//struct CameraComponents {
//	Player* Players;
//};
//
//struct ModelComponents {
//	IW::Transform* Transform;
//	IW::Model* Model;
//};
//
//int GameLayer::Initialize(
//	IW::InitOptions& options)
//{
//	//Asset->Load<IW::Mesh>("res/quad.obj");
//	//   |
//	//  \ /
//	//Asset->Load<IW::Material>("res/quad.mat");
//	//   |
//	//  \ /
//	//Asset->Load<IW::Shader>("res/sandbox.shader");
//	
//	iw::ref<IW::IPipeline> shader = Renderer->CreatePipeline("assets/shaders/sandboxvs.glsl", "assets/shaders/sandboxfs.glsl");
//	iw::ref<IW::IPipeline> shader_line = Renderer->CreatePipeline("assets/shaders/sandbox_line_vs.glsl", "assets/shaders/sandbox_line_fs.glsl");
//
//	// Making line mesh
//	// Model data
//	iw::vector3 verts[2] = {
//		iw::vector3(0, 0, -1), 
//		iw::vector3(4, 0, -1)
//	};
//
//	iw::vector4 color[2] = {
//		iw::vector4(1, 1, 1, 1),
//		iw::vector4(1, 1, 1, 0)
//	};
//
//	unsigned int index[2] = {
//		0, 1
//	};
//
//	iw::ref<IW::Texture> tex = Asset->Load<IW::Texture>("textures/metal/albedo.jpg");
//	tex->Initialize(Renderer->Device);
//
//	iw::ref<IW::Material> lineMaterial(new IW::Material(shader_line));
//
//	iw::ref<IW::Material> quadMaterial(new IW::Material(shader));
//	//quadMaterial->SetFloats("color", &iw::vector3(1, 0, 0), 3);
//	quadMaterial->SetTexture("albedoMap", tex);
//
//	iw::ref<IW::Material> circleMaterial(new IW::Material(shader));
//	//circleMaterial->SetFloats("color", &iw::vector3(1, 1, 1), 3);
//
//	line = new IW::Mesh(IW::LINES);
//	line->SetMaterial(lineMaterial);
//	line->SetVertices(2, verts);
//	line->SetColors  (2, color);
//	line->SetIndices (2, index);
//	line->Initialize(Renderer->Device);
//
//	// Making quad mesh
//	auto qmodel = Asset->Load<IW::Model>("quad.obj");
//	
//	iw::vector2 uvs[4] = {
//		iw::vector2(.1f, 0),
//		iw::vector2(.1f, .1f),
//		iw::vector2(0, .1f),
//		iw::vector2(0, 0)
//	};
//
//	qmodel->Meshes->SetUVs(4, uvs);
//	qmodel->Meshes->Initialize(Renderer->Device);
//
//	// Making circle mesh
//	auto cmodel = Asset->Load<IW::Model>("circle.obj");
//	cmodel->Meshes->Initialize(Renderer->Device);
//
//	qmodel->Meshes->SetMaterial(quadMaterial);
//	cmodel->Meshes->SetMaterial(circleMaterial);
//
//	// give enemy system circle mesh
//
//	PushSystem<EnemySystem>(&cmodel->Meshes[0]);
//
//	// Camera & entities
//
//	IW::Camera* ortho = new IW::OrthographicCamera(64, 36, -100, 100);
//	ortho->Rotation = iw::quaternion::from_euler_angles(0, iw::PI, 0);
//
//	IW::Entity camera = Space->CreateEntity<IW::CameraController>();
//	Space->SetComponentData<IW::CameraController>(camera, ortho);
//
//	IW::Entity player = Space->CreateEntity<IW::Transform, IW::Model, Player, IwPhysics::AABB2D>();
//	Space->SetComponentData<IW::Transform>    (player, iw::vector3(10, 0, 0), iw::vector3::one, iw::quaternion::from_euler_angles(0, 0, 0));
//	Space->SetComponentData<IW::Model>  (player, &qmodel->Meshes[0], 1U);
//	Space->SetComponentData<Player>           (player, 10.0f, 100.0f, 0.1666f, 0.1f);
//	Space->SetComponentData<IwPhysics::AABB2D>(player, iw::vector2(-1), iw::vector2(1));
//
//	for (float x = 4; x < 7; x++) {
//		for (float y = 5; y < 6; y++) {
//			IW::Entity enemy = Space->CreateEntity<IW::Transform, IW::Model, Enemy, IwPhysics::AABB2D>();
//			Space->SetComponentData<IW::Transform>    (enemy, iw::vector3(x * 3 - 15, y * 3 - 15, 0));
//			Space->SetComponentData<IW::Model>  (enemy, &qmodel->Meshes[0], 1U);
//			Space->SetComponentData<Enemy>            (enemy, SPIN, 3.0f, 0.05f, 0.025f, 0.025f);
//			Space->SetComponentData<IwPhysics::AABB2D>(enemy, iw::vector2(-1), iw::vector2(1));
//		}
//	}
//
//	return Layer::Initialize(options);
//}
//
//void GameLayer::PostUpdate() {
//	line->Vertices[1] *= iw::quaternion::from_euler_angles(0, 0, IW::Time::DeltaTime());
//	line->Update(Renderer->Device);
//
//	for (auto camera : Space->Query<IW::CameraController>()) {
//		auto [controller] = camera.Components.TieTo<IW::CameraController>();
//		
//		Renderer->BeginScene(controller->Camera);
//
//		for (auto entity : Space->Query<IW::Transform, IW::Model>()) {
//			auto [transform, model] = entity.Components.Tie<ModelComponents>();
//			for (int i = 0; i < model->MeshCount; i++) {
//				Renderer->DrawMesh(transform, model->Meshes);
//			}
//		}
//
//		for (auto entity : Space->Query<IW::Transform, Bullet>()) {
//			auto [transform] = entity.Components.TieTo<IW::Transform>();
//			Renderer->DrawMesh(transform, line);
//		}
//
//		Renderer->EndScene();
//	}
//}
//
//void GameLayer::FixedUpdate() {
//
//}
//
//void GameLayer::ImGui() {
//	ImGui::Begin("Game layer");
//
//	ImGui::Text("sdfsdf");
//
//	for (auto entity : Space->Query<Player>()) {
//		auto [player] = entity.Components.TieTo<Player>();
//		
//		float cooldown = player->DashCooldown + player->DashTime;
//
//		ImGui::Text("Dash time: %f",
//			player->DashTime > 0 ? player->DashTime : 0);
//
//		ImGui::Text("Dash cooldown time: %f",
//			cooldown > 0 ? cooldown : 0);
//	}
//
//	int i = 0;
//	for (auto entity : Space->Query<Bullet>()) {
//		++i;
//	}
//
//	ImGui::Text("Bullet count: %i", i);
//
//	i = 0;
//	for (auto entity : Space->Query<Enemy>()) {
//		++i;
//	}
//
//	ImGui::Text("Enemy count: %i", i);
//
//	ImGui::End();
//}
