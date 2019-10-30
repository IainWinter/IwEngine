#include "Layers/GameLayer3D.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"

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
	pipeline = Renderer.CreatePipeline("res/elvs.glsl", "res/elfs.glsl");

	iwu::ref<IW::ModelData> data = Asset.Load<IW::ModelData>("res/cube2.obj");
	material = Asset.Load<IW::Material>("Material");
	material->SetColor("diffuse", IW::Color(.67f, .3f, .4f, .6f));
	material->SetColor("specular", IW::Color(1.0f, 1.0f, 1.0f, .1f));

	//iwu::ref<IW::Material> leafMaterial(new IW::Material(pipeline));
	//leafMaterial->SetProperty("color", iwm::vector3(.2, .6, .1));

	//iwu::ref<IW::Material> material(new IW::Material(pipeline));
	//material->SetProperty("color", iwm::vector3(.6, .4, .3));

	treeMeshs = new IW::Mesh[2];
	for (size_t i = 0; i < data->MeshCount; i++) {
		treeMeshs[i].SetVertices(data->Meshes[i].VertexCount, data->Meshes[i].Vertices);
		treeMeshs[i].SetNormals (data->Meshes[i].VertexCount, data->Meshes[i].Normals);
		treeMeshs[i].SetIndices (data->Meshes[i].FaceCount,   data->Meshes[i].Faces);
		treeMeshs[i].Compile(Renderer.Device);
	}

	material->Pipeline = pipeline;
	//leafMaterial->Pipeline = pipeline;

	treeMeshs[0].SetMaterial(material);
	//treeMeshs[1].SetMaterial(leafMaterial);
	//treeMeshs[2].SetMaterial(material);
	
	IW::Camera* ortho = new IW::OrthographicCamera(64, 36, -100, 100);
	ortho->Rotation = iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0);

	IW::Camera* camera = new IW::PerspectiveCamera(1.17f, 1.778f, 0.001f, 1000.0f);
	//camera->Rotation = iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0);

	IwEntity::Entity player = Space.CreateEntity<IW::Transform, IwEngine::CameraController>();
	Space.SetComponentData<IW::Transform>             (player, iwm::vector3(0, 0, 0));
	Space.SetComponentData<IwEngine::CameraController>(player, camera);

	IwEntity::Entity tree = Space.CreateEntity<IW::Transform, IwEngine::Model>();
	Space.SetComponentData<IW::Transform>(tree, iwm::vector3(0, 0, 5));
	Space.SetComponentData<IwEngine::Model>(tree, treeMeshs, 1U);

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

		Renderer.BeginScene(controller->Camera);

		pipeline->GetParam("lightPos")->SetAsVec3(transform->Position);
		pipeline->GetParam("viewPos") ->SetAsVec3(transform->Position);

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

	ImGui::ColorPicker4("Ambient Color", (float*)&material->GetColor("ambient"));
	ImGui::ColorPicker4("Diffuse Color", (float*)&material->GetColor("diffuse"));
	ImGui::ColorPicker4("Specular Color", (float*)&material->GetColor("specular"));

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

