#include "Layers/GameLayer3D.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/engine/Time.h"

GameLayer3D::GameLayer3D(
	IwEntity::Space& space, 
	IW::Renderer& renderer,
	IW::AssetManager& asset)
	: Layer(space, renderer, asset, "Game")
{}

int GameLayer3D::Initialize(
	IwEngine::InitOptions& options)
{
	iwu::ref<IW::IPipeline> pipeline = Renderer.CreatePipeline("res/sandboxvs.glsl", "res/sandboxfs.glsl");

	IW::ModelData* treeData = Asset.Load<IW::ModelData>("res/tree.obj");

	iwu::ref<IW::Material> leafMaterial(new IW::Material(pipeline));
	leafMaterial->SetProperty("color", iwm::vector3(.2, .6, .1));

	iwu::ref<IW::Material> treeMaterial(new IW::Material(pipeline));
	treeMaterial->SetProperty("color", iwm::vector3(.6, .4, .3));

	treeMeshs = new IW::Mesh[2];
	for (size_t i = 0; i < treeData->MeshCount; i++) {
		treeMeshs[i].SetVertices(treeData->Meshes[i].VertexCount, treeData->Meshes[i].Vertices);
		treeMeshs[i].SetNormals (treeData->Meshes[i].VertexCount, treeData->Meshes[i].Normals);
		treeMeshs[i].SetIndices (treeData->Meshes[i].FaceCount,   treeData->Meshes[i].Faces);
		treeMeshs[i].Compile(Renderer.Device);
	}

	treeMeshs[0].SetMaterial(treeMaterial);
	treeMeshs[1].SetMaterial(leafMaterial);
	
	IW::Camera* ortho = new IW::OrthographicCamera(64, 36, -100, 100);
	ortho->Rotation = iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0);

	IW::Camera* camera = new IW::PerspectiveCamera(1.17f, 1.778f, 0.001f, 1000.0f);
	//camera->Rotation = iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0);

	IwEntity::Entity player = Space.CreateEntity<IW::Transform, IwEngine::CameraController>();
	Space.SetComponentData<IW::Transform>             (player, iwm::vector3(0, 0, 0));
	Space.SetComponentData<IwEngine::CameraController>(player, camera);

	IwEntity::Entity tree = Space.CreateEntity<IW::Transform, IwEngine::Model>();
	Space.SetComponentData<IW::Transform>(tree, iwm::vector3(0, 0, 5));
	Space.SetComponentData<IwEngine::Model>(tree, treeMeshs, 2U);

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

float cooldown = 0.0f;

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

		if (cooldown > 0.0) {
			cooldown -= IwEngine::Time::DeltaTime();
		}

		else if (IwInput::Mouse::ButtonDown(IwInput::MOUSE_L_BUTTON)) {
			cooldown = 0.1f;
			IwEntity::Entity tree = Space.CreateEntity<IW::Transform, IwEngine::Model>();
			Space.SetComponentData<IW::Transform>  (tree, transform->Position + transform->Forward() * 4);
			Space.SetComponentData<IwEngine::Model>(tree, treeMeshs, 2U);
		}

		transform->Rotation *= iwm::quaternion::create_from_axis_angle(iwm::vector3::unit_y, mouse.x * IwEngine::Time::DeltaTime())
			* iwm::quaternion::create_from_axis_angle(-iwm::vector3::unit_x, mouse.y * IwEngine::Time::DeltaTime());

		mouse = 0;

		transform->Position += movement * 5 * IwEngine::Time::DeltaTime();

		controller->Camera->Position = transform->Position;
		controller->Camera->Rotation = transform->Rotation;

		Renderer.BeginScene(controller->Camera);

		for (auto tree : Space.Query<IW::Transform, IwEngine::Model>()) {
			auto [transform, model] = tree.Components.Tie<TreeComponents>();
			for (size_t i = 0; i < model->MeshCount; i++) {
				Renderer.DrawMesh(transform, &model->Meshes[i]);
			}
		}

		Renderer.EndScene();
	}
}

bool GameLayer3D::On(
	IwEngine::MouseMovedEvent& event)
{
	mouse.x = event.DeltaX;
	mouse.y = event.DeltaY;
	return false;
}

