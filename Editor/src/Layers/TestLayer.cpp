#include "Layers/TestLayer.h"
#include "Events/ActionEvents.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include "imgui/imgui.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/graphics/PointLight.h"

namespace iw {
	struct MeshComponents {
		Transform* Transform;
		Mesh* Model;
	};

	struct CameraControllerComponents {
		Transform* Transform;
		CameraController* Model;
	};

	TestLayer::TestLayer()
		 : Layer("Test")
	{}

	int TestLayer::Initialize() {
		shader = Asset->Load<Shader>("shaders/test_pbr.shader");
		Renderer->InitShader(shader, CAMERA | LIGHTS);

		Mesh* mesh = MakeIcosphere(5);

		srand(time(nullptr));

		for (int x = -4; x < 5; x++) {
			Entity entity = Space->CreateEntity<Transform, Mesh>();

			Transform* transform  = entity.SetComponent<Transform>(iw::vector3(x * 2.5f, 0, 10));
			Mesh*      sphere     = entity.SetComponent<Mesh>();

			*sphere = mesh->Instance();

			sphere->Material = REF<Material>(shader);
			sphere->Material->Set("albedo", iw::vector3(
				rand() / (float)RAND_MAX, 
				rand() / (float)RAND_MAX, 
				rand() / (float)RAND_MAX)
			);

			sphere->Material->Set("reflectance", rand() / (float)RAND_MAX);
			sphere->Material->Set("roughness",   rand() / (float)RAND_MAX);
			sphere->Material->Set("metallic",    rand() / (float)RAND_MAX);

			sphere->Initialize(Renderer->Device);
		}

		camera = new PerspectiveCamera(1.17f, 1.77f, .01f, 100.0f);

		Entity rig = Space->CreateEntity<Transform, CameraController>();
		
		Transform* transform = rig.SetComponent<Transform>();
		                       rig.SetComponent<CameraController>(camera);


		camera->SetTrans(transform);

		light  = new iw::PointLight(30.f, 1.0f);
		light2 = new iw::PointLight(30.f, 1.0f);
		dirLight = new iw::DirectionalLight();

		scene = new Scene();

		scene->Camera = camera;
		scene->PointLights.push_back(light);
		scene->PointLights.push_back(light2);

		delete mesh;

		return Layer::Initialize();
	}

	void TestLayer::PostUpdate() {
		Renderer->BeginScene(scene);

		for (auto entity : Space->Query<Transform, Mesh>()) {
			auto [transform, sphere] = entity.Components.Tie<MeshComponents>();
			Renderer->DrawMesh(transform, sphere);
		}

		Renderer->EndScene();
	}

	void TestLayer::ImGui() {
		ImGui::Begin("Test");

		ImGui::SliderFloat3("Light pos", (float*)&light->Position(), -10, 10);
		ImGui::SliderFloat("Light rad", &light->Radius(), 0, 100);

		ImGui::SliderFloat3("Light 2 pos", (float*)&light2->Position(), -10, 10);
		ImGui::SliderFloat("Light 2 rad", &light2->Radius(), 0, 100);

		for (auto entity : Space->Query<Transform, Mesh>()) {
			auto [transform, sphere] = entity.Components.Tie<MeshComponents>();

			std::stringstream ss;
			ss << "Reflectance " << entity.Index;

			ImGui::SliderFloat(ss.str().c_str(), sphere->Material->Get<float>("reflectance"), 0, 1);
			
			ss = std::stringstream();
			ss << "Roughness " << entity.Index;
			
			ImGui::SliderFloat(ss.str().c_str(), sphere->Material->Get<float>("roughness"), 0, 1);

			ss = std::stringstream();
			ss << "Metallic " << entity.Index;

			ImGui::SliderFloat(ss.str().c_str(), sphere->Material->Get<float>("metallic"), 0, 1);
		}

		ImGui::End();
	}
}

