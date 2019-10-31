#include "Layers/GameLayer3D.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"
#include "iw/graphics/MeshFactory.h"

#include "iw/util/io/File.h"

GameLayer3D::GameLayer3D(
	IwEntity::Space& space, 
	IW::Renderer& renderer,
	IW::AssetManager& asset)
	: Layer(space, renderer, asset, "Game")
{}

iwu::ref<IW::IPipeline> pipeline;

int GameLayer3D::Initialize(
	IwEngine::InitOptions& options)
{
	pipeline = Renderer.CreatePipeline("res/pbr/pbrvs.glsl", "res/pbr/pbrfs.glsl");

	iwu::ref<IW::ModelData> data = Asset.Load<IW::ModelData>("res/cube2.obj");
	material = Asset.Load<IW::Material>("Material");

	meshes = new IW::Mesh[1];
	for (size_t i = 0; i < data->MeshCount; i++) {
		meshes[i].SetVertices(data->Meshes[i].VertexCount, data->Meshes[i].Vertices);
		meshes[i].SetNormals (data->Meshes[i].VertexCount, data->Meshes[i].Normals);
		meshes[i].SetIndices (data->Meshes[i].FaceCount,   data->Meshes[i].Faces);
		meshes[i].Compile(Renderer.Device);
	}

	//meshes = IW::mesh_factory::create_icosphere(5);
	//meshes->Compile(Renderer.Device);

	//material = std::make_shared<IW::Material>(pipeline);

	//material->SetFloats("albedo", &iwm::vector3(1.0f, 0.85f, 0.57f), 3);
	//material->SetFloat ("metallic", 1.0f);
	//material->SetFloat ("roughness", 0.6f);
	//material->SetFloat ("ao", 1.0f);

	material->Pipeline = pipeline;

	meshes[0].SetMaterial(material);
	
	lightPositions[0] = iwm::vector3( 5,  5, -5);
	lightPositions[1] = iwm::vector3(-5,  5, -5);
	lightPositions[2] = iwm::vector3( 5, -5, -5);
	lightPositions[3] = iwm::vector3(-5, -5, -5);

	lightColors[0] = iwm::vector3(1);
	lightColors[1] = iwm::vector3(1, 0, 0);
	lightColors[2] = iwm::vector3(0, 1, 0);
	lightColors[3] = iwm::vector3(0, 0, 1);

	IW::Camera* ortho = new IW::OrthographicCamera(64, 36, -100, 100);
	ortho->Rotation = iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0);

	IW::Camera* camera = new IW::PerspectiveCamera(1.17f, 1.778f, 0.001f, 1000.0f);
	//camera->Rotation = iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0);

	IwEntity::Entity player = Space.CreateEntity<IW::Transform, IwEngine::CameraController>();
	Space.SetComponentData<IW::Transform>             (player, iwm::vector3(0, 0, 0));
	Space.SetComponentData<IwEngine::CameraController>(player, camera);

	for (int x = -5; x < 10; x++) {
		for (int y = -5; y < 10; y++) {
			IwEntity::Entity tree = Space.CreateEntity<IW::Transform, IwEngine::Model>();
			Space.SetComponentData<IW::Transform>(tree, iwm::vector3(x, y, 5), iwm::vector3(.5));
			Space.SetComponentData<IwEngine::Model>(tree, meshes, 1U);
		}
	}

	return 0;
}

struct PlayerComponents {
	IW::Transform* Transform;
	IwEngine::CameraController* Controller;
};

struct TreeComponents {
	IW::Transform* Transform;
	IwEngine::Model* Model;
};

void GameLayer3D::PostUpdate() {
	for (auto entity : Space.Query<IW::Transform, IwEngine::CameraController>()) {
		auto [transform, controller] = entity.Components.Tie<PlayerComponents>();

		iwm::vector3 movement;
		if (IwInput::Keyboard::KeyDown(IwInput::W)) {
			movement += transform->Forward();
		}

		if (IwInput::Keyboard::KeyDown(IwInput::S)) {
			movement -= transform->Forward();
		}

		if (IwInput::Keyboard::KeyDown(IwInput::A)) {
			movement += transform->Right();
		}

		if (IwInput::Keyboard::KeyDown(IwInput::D)) {
			movement -= transform->Right();
		}

		if (IwInput::Keyboard::KeyDown(IwInput::SPACE)) {
			movement += transform->Up();
		}

		if (IwInput::Keyboard::KeyDown(IwInput::LEFT_SHIFT)) {
			movement -= transform->Up();
		}

		transform->Position += movement * 5 * IwEngine::Time::DeltaTime();
		controller->Camera->Position = transform->Position;

		lightPositions[0] = transform->Position;

		Renderer.BeginScene(controller->Camera);

		pipeline->GetParam("lightPositions")->SetAsFloats(&lightPositions, 4, 3);
		pipeline->GetParam("lightColors")   ->SetAsFloats(&lightColors, 4, 3);

		pipeline->GetParam("camPos") ->SetAsFloats(&(transform->Position + transform->Forward()), 3);

		for (auto tree : Space.Query<IW::Transform, IwEngine::Model>()) {
			auto [transform, model] = tree.Components.Tie<TreeComponents>();
			for (size_t i = 0; i < model->MeshCount; i++) {
				Renderer.DrawMesh(transform, &model->Meshes[i]);
			}
		}

		Renderer.EndScene();
	}
}

void GameLayer3D::ImGui() {
	ImGui::Begin("Game layer");

	//ImGui::ColorPicker3("Color", (float*)std::get<0>(material->GetFloats("albedo")));
	//ImGui::SliderFloat("Metallic", (float*)material->GetFloat("metallic"), 0, 1);
	//ImGui::SliderFloat("Roughness", (float*)material->GetFloat("roughness"), 0.3f, 1);
	//ImGui::SliderFloat("AO", (float*)material->GetFloat("ao"), 0, 1);
	//ImGui::ColorPicker4("Diffuse Color", (float*)&material->GetColor("diffuse"));
	//ImGui::ColorPicker4("Specular Color", (float*)&material->GetColor("specular"));

	ImGui::End();
}

bool GameLayer3D::On(
	IwEngine::MouseMovedEvent& event)
{
	for (auto entity : Space.Query<IW::Transform, IwEngine::CameraController>()) {
		auto [transform, controller] = entity.Components.Tie<PlayerComponents>();

		float pitch = event.DeltaY * 0.0005f;
		float yaw   = event.DeltaX * 0.0005f;

		iwm::quaternion deltaP = iwm::quaternion::create_from_axis_angle(-transform->Right(), pitch);
		iwm::quaternion deltaY = iwm::quaternion::create_from_axis_angle( iwm::vector3::unit_y, yaw);

		transform->Rotation *= deltaP * deltaY;

		controller->Camera->Rotation = transform->Rotation;
	}

	return false;
}

