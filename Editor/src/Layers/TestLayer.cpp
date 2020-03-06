#include "Layers/TestLayer.h"
#include "Events/ActionEvents.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include "imgui/imgui.h"
#include "iw/engine/Time.h"

// look at me

namespace iw {
	struct ModelComponents {
		Transform* Transform;
		Model* Model;
	};

	TestLayer::TestLayer()
		 : Layer("Test")
	{}

	int TestLayer::Initialize() {
		ref<Shader> shader = Asset->Load<Shader>("shaders/test.shader");
		Renderer->InitShader(shader, CAMERA);

		sphere = MakeIcosphere(1);
		sphere->Material = REF<Material>(shader);
		sphere->Material->Set("albedo", Color(1));
		//sphere->Material->SetTexture("albedoMap", Asset->Load<Texture>("textures/forest/floor/albedo.jpg"));

		sphere->Normals = nullptr;

		sphere->Initialize(Renderer->Device);

		camera = new PerspectiveCamera(1.17f, 1.77f, .01f, 100.0f);
		transform = new Transform();

		camera->Position.z = -5;

		return Layer::Initialize();
	}

	void TestLayer::PostUpdate() {
		transform->Rotation *= iw::quaternion::from_euler_angles(iw::Time::DeltaTime());

		Renderer->BeginScene();
		Renderer->SetCamera(camera);
		Renderer->DrawMesh(transform, sphere);
		Renderer->EndScene();
	}

	void TestLayer::ImGui() {
		ImGui::Begin("Test");

		ImGui::SliderFloat3("pos", (float*)&transform->Position, -10, 10);

		ImGui::End();
	}
}

