#include "Layers/RayMarchingLayer.h"
#include "iw/engine/Components/CameraController.h"
#include "Systems/EditorCameraController.h"

namespace iw {
	RayMarchingLayer::RayMarchingLayer()
		 : Layer("Ray Marching")
	{
		srand(time(nullptr));
	}

	int RayMarchingLayer::Initialize() {
		// Shaders

		shader = Asset->Load<Shader>("shaders/rt.shader");

		Renderer->InitShader(shader);

		// Camera controller

		//camera = new PerspectiveCamera(1.17f, 1.77f, .01f, 100.0f);

		//Entity entity = Space->CreateEntity<Transform, CameraController>();

		//Transform* transform = entity.SetComponent<Transform>(vector3(0, 6, -5));
		//	                   entity.SetComponent<CameraController>(camera);

		//camera->SetTrans(transform);

		// Systems

		//PushSystem<EditorCameraController>();
		
		return Layer::Initialize();
	}

	void RayMarchingLayer::PostUpdate() {
		//Renderer->BeginScene(camera);

		Renderer->ApplyFilter(shader, nullptr);

		//Renderer->EndScene();
	}

	void RayMarchingLayer::ImGui() {
		//ImGui::Begin("Test");

		//ImGui::End();
	}
}

