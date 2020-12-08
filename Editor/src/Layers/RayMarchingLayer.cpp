#include "Layers/RayMarchingLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

namespace iw {
	RayMarchingLayer::RayMarchingLayer()
		 : Layer("Ray Marching")
		, shader(nullptr)
		, camera(nullptr)
	{
		srand(time(nullptr));
	}

	int RayMarchingLayer::Initialize() {
		shader = Asset->Load<Shader>("shaders/rt.shader");

		camera = PushSystem<EditorCameraControllerSystem>()->GetCamera();
		
		if (int err = Layer::Initialize()) {
			return err;
		}

		camera->SetPosition(vector3(0, 2, 10));
		camera->SetRotation(quaternion::from_look_at(camera->Position()).inverted());

		return 0;
	}

	void RayMarchingLayer::PostUpdate() {
		Renderer->ApplyFilter(shader, nullptr, nullptr, camera);
	}
}

