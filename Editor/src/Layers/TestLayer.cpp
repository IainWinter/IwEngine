#include "Layers/TestLayer.h"
#include "Events/ActionEvents.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include "imgui/imgui.h"
#include "iw/engine/Time.h"

namespace iw {
	struct MeshComponents {
		Transform* Transform;
		Mesh* Model;
	};

	TestLayer::TestLayer()
		 : Layer("Test")
	{}

	int TestLayer::Initialize() {
		shader = Asset->Load<Shader>("shaders/test_pbr.shader");
		Renderer->InitShader(shader, CAMERA);

		Mesh* mesh = MakeIcosphere(5);

		for (int x = -4; x < 5; x++) {
			Entity entity = Space->CreateEntity<Transform, Mesh>();

			Transform* transform  = entity.SetComponent<Transform>(iw::vector3(x * 2.5f, 0, 7.5f));
			Mesh*      sphere     = entity.SetComponent<Mesh>();

			*sphere = mesh->Instance();

			sphere->Material = REF<Material>(shader);
			sphere->Material->Set("albedo", iw::vector3(
				rand() / (float)RAND_MAX, 
				rand() / (float)RAND_MAX, 
				rand() / (float)RAND_MAX)
			);

			sphere->Initialize(Renderer->Device);
		}

		camera = new PerspectiveCamera(1.17f, 1.77f, .01f, 100.0f);
		camera->Position.z = -5;

		lightPos = iw::vector3(0, 4, 5);

		delete mesh;

		return Layer::Initialize();
	}

	void TestLayer::PostUpdate() {
		Renderer->BeginScene(camera);
		
		shader->Program->GetParam("lightPos")->SetAsFloats(&lightPos, 3);

		for (auto entity : Space->Query<Transform, Mesh>()) {
			auto [transform, sphere] = entity.Components.Tie<MeshComponents>();
			Renderer->DrawMesh(transform, sphere);
		}

		Renderer->EndScene();
	}

	void TestLayer::ImGui() {
		ImGui::Begin("Test");

		ImGui::SliderFloat3("Light pos", (float*)&lightPos, -10, 10);

		for (auto entity : Space->Query<Transform, Mesh>()) {
			auto [transform, sphere] = entity.Components.Tie<MeshComponents>();

			std::stringstream ss;
			ss << "Reflectance " << entity.Index;

			ImGui::SliderFloat(ss.str().c_str(), sphere->Material->Get<float>("reflectance"), 0, 1);
			
			ss = std::stringstream();
			ss << "Smoothness " << entity.Index;
			
			ImGui::SliderFloat(ss.str().c_str(), sphere->Material->Get<float>("smoothness"), 0, 1);

			ss = std::stringstream();
			ss << "Metallic " << entity.Index;

			ImGui::SliderFloat(ss.str().c_str(), sphere->Material->Get<float>("metallic"), 0, 1);
		}

		ImGui::End();
	}
}

