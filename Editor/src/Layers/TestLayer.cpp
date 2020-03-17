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
		Mesh* Mesh;
	};

	struct ModelComponents {
		Transform* Transform;
		Model* Model;
	};

	struct CameraControllerComponents {
		Transform* Transform;
		CameraController* Model;
	};

	TestLayer::TestLayer()
		 : Layer("Test")
	{
		srand(time(nullptr));
	}

	int TestLayer::Initialize() {
		// Shaders

		                    shader    = Asset->Load<Shader>("shaders/pbr.shader");
		ref<Shader> dirShadowShader   = Asset->Load<Shader>("shaders/lights/directional.shader");
		ref<Shader> pointShadowShader = Asset->Load<Shader>("shaders/lights/point.shader");

		Renderer->InitShader(shader,          CAMERA | SHADOWS | LIGHTS);
		Renderer->InitShader(dirShadowShader, CAMERA);
		Renderer->InitShader(pointShadowShader);

		// Directional light shadow map textures & target

		ref<Texture> dirShadowColor = ref<Texture>(new Texture(1024, 1024, TEX_2D, RG,    FLOAT, BORDER));
		ref<Texture> dirShadowDepth = ref<Texture>(new Texture(1024, 1024, TEX_2D, DEPTH, FLOAT, BORDER));

		ref<RenderTarget> dirShadowTarget = REF<RenderTarget>(1024, 1024);
		dirShadowTarget->AddTexture(dirShadowColor);
		dirShadowTarget->AddTexture(dirShadowDepth);

		dirShadowTarget->Initialize(Renderer->Device);

		// Point light shadow map textures & target

		ref<Texture> pointShadowDepth = ref<Texture>(new Texture(1024, 1024, TEX_CUBE, DEPTH, FLOAT, EDGE));

		ref<RenderTarget> pointShadowTarget = REF<RenderTarget>(1024, 1024, true);
		pointShadowTarget->AddTexture(pointShadowDepth);

		pointShadowTarget->Initialize(Renderer->Device);

		// Meshes

		Mesh*      sphere = MakeIcosphere(5);
		ref<Model> plane  = Asset->Load<Model>("models/block/level1.dae");

		for (int x = -4; x < 5; x++) {
			Entity entity = Space->CreateEntity<Transform, Mesh>();

			Transform* transform = entity.SetComponent<Transform>(iw::vector3(x * 2.5f, 0, 10));
			Mesh*      mesh      = entity.SetComponent<Mesh>();

			*mesh = sphere->Instance();

			mesh->Material = REF<Material>(shader);
			mesh->Material->Set("albedo", iw::vector4(
				rand() / (float)RAND_MAX,
				rand() / (float)RAND_MAX,
				rand() / (float)RAND_MAX,
				1.0f)
			);

			mesh->Material->Set("reflectance", rand() / (float)RAND_MAX);
			mesh->Material->Set("roughness",   rand() / (float)RAND_MAX);
			mesh->Material->Set("metallic",    rand() / (float)RAND_MAX);

			mesh->Material->SetTexture("shadowMap",  dirShadowTarget  ->Tex(0));
			mesh->Material->SetTexture("shadowMap2", pointShadowTarget->Tex(0));

			mesh->Initialize(Renderer->Device);
		}

		// Floor

		{
			Entity entity = Space->CreateEntity<Transform, Model>();

			Transform* transform = entity.SetComponent<Transform>(iw::vector3(0, -2, 10));
			Model*      model      = entity.SetComponent<Model>(*plane);

			for (size_t i = 0; i < model->MeshCount; i++) {
				Mesh& mesh = model->Meshes[i];

				mesh.Material->SetShader(shader);

				//mesh.Material = REF<Material>(shader);
				mesh.Material->Set("albedo", iw::vector4(1));
					
				mesh.Material->Set("reflectance", 0.0f);
				mesh.Material->Set("roughness", 1.0f);
				mesh.Material->Set("metallic", 0.0f);
					
				mesh.Material->SetTexture("shadowMap", dirShadowTarget->Tex(0));
				mesh.Material->SetTexture("shadowMap2", pointShadowTarget->Tex(0));
					
				mesh.Initialize(Renderer->Device);
			}

		}

		// Camera controller

		{
			camera = new PerspectiveCamera(1.17f, 1.77f, .01f, 100.0f);

			Entity entity = Space->CreateEntity<Transform, CameraController>();

			Transform* transform = entity.SetComponent<Transform>();
			                       entity.SetComponent<CameraController>(camera);

			camera->SetTrans(transform);
		}

		// lights

		light = new PointLight(12.0f, 1.0f);
		light->SetPosition(iw::vector3(-4.5f, 8.0f, 10.0f));
		light->SetShadowShader(pointShadowShader);
		light->SetShadowTarget(pointShadowTarget);

		light2 = new PointLight(12.0f, 1.0f);
		light2->SetPosition(iw::vector3( 4.5f, 8.0f, 10.0f));

		dirLight = new DirectionalLight();
		dirLight->SetRotation(quaternion(0.872f, 0.0f, 0.303f, 0.384f));
		dirLight->SetShadowShader(dirShadowShader);
		dirLight->SetShadowTarget(dirShadowTarget);

		// scene

		scene = new Scene();
		scene->Camera = camera;
		scene->PointLights.push_back(light);
		//scene->PointLights.push_back(light2);
		scene->DirectionalLights.push_back(dirLight);

		delete sphere;

		return Layer::Initialize();
	}

	void TestLayer::PostUpdate() {
		if (dirLight->Outdated()) {
			Renderer->BeginShadowCast(dirLight);

			for (auto entity : Space->Query<Transform, Mesh>()) {
				auto [transform, mesh] = entity.Components.Tie<MeshComponents>();
				Renderer->DrawMesh(transform, mesh);
			}

			for (auto entity : Space->Query<Transform, Model>()) {
				auto [transform, model] = entity.Components.Tie<ModelComponents>();
				for (size_t i = 0; i < model->MeshCount; i++) {
					Mesh& mesh = model->Meshes[i];
					Renderer->DrawMesh(transform, &mesh);
				}
			}

			Renderer->EndShadowCast();
		}

		if (light->Outdated()) {
			Renderer->BeginShadowCast(light);

			for (auto entity : Space->Query<Transform, Mesh>()) {
				auto [transform, mesh] = entity.Components.Tie<MeshComponents>();
				Renderer->DrawMesh(transform, mesh);
			}

			for (auto entity : Space->Query<Transform, Model>()) {
				auto [transform, model] = entity.Components.Tie<ModelComponents>();
				for (size_t i = 0; i < model->MeshCount; i++) {
					Mesh& mesh = model->Meshes[i];
					Renderer->DrawMesh(transform, &mesh);
				}
			}

			Renderer->EndShadowCast();
		}

		Renderer->BeginScene(scene);

		for (auto entity : Space->Query<Transform, Mesh>()) {
			auto [transform, mesh] = entity.Components.Tie<MeshComponents>();
			Renderer->DrawMesh(transform, mesh);
		}

		for (auto entity : Space->Query<Transform, Model>()) {
			auto [transform, model] = entity.Components.Tie<ModelComponents>();
			for (size_t i = 0; i < model->MeshCount; i++) {
				Mesh& mesh = model->Meshes[i];
				Renderer->DrawMesh(transform, &mesh);
			}
		}

		Renderer->EndScene();
	}

	void TestLayer::ImGui() {
		ImGui::Begin("Test");

		float r = light->Radius();
		vector3 pos = light->Position();

		ImGui::SliderFloat3("Light pos", (float*)&pos, -20, 20);
		ImGui::SliderFloat("Light rad", &r, 0, 100);

		light->SetRadius(r);
		light->SetPosition(pos);

		//ImGui::SliderFloat3("Light 2 pos", (float*)&light2->Position(), -10, 10);
		//ImGui::SliderFloat("Light 2 rad", &light2->Radius(), 0, 100);

		quaternion rot = dirLight->Rotation();

		ImGui::SliderFloat4("Dir rot", (float*)&rot, 0, 1);

		rot.normalize();

		dirLight->SetRotation(rot);

		if (ImGui::Button("Shuffle properties")) {
			for (auto entity : Space->Query<Transform, Mesh>()) {
				auto [transform, mesh] = entity.Components.Tie<MeshComponents>();

				mesh->Material->Set("albedo", iw::vector4(
					rand() / (float)RAND_MAX,
					rand() / (float)RAND_MAX,
					rand() / (float)RAND_MAX,
					1.0f)
				);

				mesh->Material->Set("reflectance", rand() / (float)RAND_MAX);
				mesh->Material->Set("roughness",   rand() / (float)RAND_MAX);
				mesh->Material->Set("metallic",    rand() / (float)RAND_MAX);
			}
		}

		//for (auto entity : Space->Query<Transform, Mesh>()) {
		//	auto [transform, mesh] = entity.Components.Tie<MeshComponents>();

		//	std::stringstream ss;
		//	ss << "Reflectance " << entity.Index;

		//	ImGui::SliderFloat(ss.str().c_str(), mesh->Material->Get<float>("reflectance"), 0, 1);
		//	
		//	ss = std::stringstream();
		//	ss << "Roughness " << entity.Index;
		//	
		//	ImGui::SliderFloat(ss.str().c_str(), mesh->Material->Get<float>("roughness"), 0, 1);

		//	ss = std::stringstream();
		//	ss << "Metallic " << entity.Index;

		//	ImGui::SliderFloat(ss.str().c_str(), mesh->Material->Get<float>("metallic"), 0, 1);
		//}

		ImGui::End();
	}
}

