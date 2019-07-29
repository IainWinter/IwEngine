#include "GameLayer.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Transform.h"
#include "iw/engine/Components/Model.h"
#include "iw/engine/Components/Camera.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/util/io/File.h"

GameLayer::GameLayer() 
	: IwEngine::Layer("Game")
	, device(nullptr)
	, pipeline(nullptr)
{}

GameLayer::~GameLayer() {
	//device->DestroyPipeline(pipeline);
	//delete device;
}

int GameLayer::Initialize() {
	device = new IwRenderer::GLDevice();

	auto vs = device->CreateVertexShader  (iwu::ReadFile("res/sandboxvs.glsl").c_str());
	auto fs = device->CreateFragmentShader(iwu::ReadFile("res/sandboxfs.glsl").c_str());

	pipeline = device->CreatePipeline(vs, fs);

	IwEntity::Entity player = space.CreateEntity();
	space.CreateComponent<IwEngine::Transform>(player);
	space.CreateComponent<IwEngine::Camera>(player, iwm::matrix4::create_orthographic(1280.0f, 720.0f, 0.001f, 1000.0f));

	IwEntity::Entity cube = space.CreateEntity();
	space.CreateComponent<IwEngine::Transform>(cube);
	space.CreateComponent<IwEngine::Model>(cube, loader.Load("res/cube.obj"), device);

	return 0;
}

void GameLayer::Update() {
	for (auto c : space.ViewComponents<IwEngine::Transform, IwEngine::Camera>()) {
		auto transform = c.GetComponent<IwEngine::Transform>();
		auto camera    = c.GetComponent<IwEngine::Camera>();

		pipeline->GetParam("proj")
			->SetAsMat4(camera.Projection);

		pipeline->GetParam("view")
			->SetAsMat4(iwm::matrix4::create_look_at(
				transform.Position,
				transform.Position + transform.Forward(),
				transform.Up()));
	}

	for (auto c : space.ViewComponents<IwEngine::Transform, IwEngine::Model>()) {
		auto transform = c.GetComponent<IwEngine::Transform>();
		auto model     = c.GetComponent<IwEngine::Model>();

		pipeline->GetParam("model")
			->SetAsMat4(transform.Transformation());

		for (int i = 0; i < model.MeshCount; i++) {
			device->SetVertexArray(model.Meshes[i].VertexArray);
			device->SetIndexBuffer(model.Meshes[i].IndexBuffer);
			device->DrawElements  (model.Meshes[i].FaceCount, 0);
		}
	}
}
