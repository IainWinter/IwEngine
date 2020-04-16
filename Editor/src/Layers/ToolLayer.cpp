#include "Layers/ToolLayer.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Systems/Debug/DrawCollidersSystem.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include "iw/graphics/Model.h"
#include "imgui/imgui.h"

#include "iw/input/Devices/Mouse.h"

namespace iw {
	struct ObjectComponents {
		Transform* Transform;
	};

	struct ModelComponents {
		Transform* Transform;
		Model* Model;
	};

	struct CameraComponents {
		Transform* Transform;
		CameraController* Controller;
	};

	ToolLayer::ToolLayer(
		Scene* scene)
		 : Layer("Toolbox")
		, scene(scene)
	{}

	Mesh particles;

	int ToolLayer::Initialize() {
		//// Selection buffer

		//ref<Texture> selectionTexture = ref<Texture>(new Texture(1024, 1024, TEX_2D, RGBA, UBYTE, EDGE));
		//selectionBuffer = ref<RenderTarget>(new RenderTarget(1024, 1024));
		//selectionBuffer->AddTexture(selectionTexture);
		//selectionBuffer->Initialize(Renderer->Device);

		// Fonts

		font = Asset->Load<Font>("fonts/arial.fnt");
		font->Initialize(Renderer->Device);

		textMesh = font->GenerateMesh("", .01f, 1);

		fontShader = Asset->Load<Shader>("shaders/font.shader");

		iw::ref<Material> textMat = REF<Material>(fontShader);

		textMat->Set("color", iw::vector3(1, .25f, 1));
		textMat->SetTexture("fontMap", font->GetTexture(0));

		textMesh.SetMaterial(textMat);
		textMesh.Data()->Initialize(Renderer->Device);

		// Meshes

		ref<Shader> shader = Asset->Load<Shader>("shaders/debug/wireframe.shader");

		Renderer->InitShader(shader, CAMERA);

		ref<Material> lightMaterial = REF<Material>(shader);
		lightMaterial->Set("color", iw::Color(1, 1, 0, 1));
		lightMaterial->SetWireframe(true);

		lightMaterial->Initialize(Renderer->Device);

		ref<Material> cameraMaterial = REF<Material>(shader);
		cameraMaterial->Set("color", iw::Color(0.1f, 0, 1, 1));
		cameraMaterial->SetWireframe(true);

		cameraMaterial->Initialize(Renderer->Device);

		MeshDescription description;
		description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));

		plightMesh = MakeIcosphere(description, 2)->MakeInstance();
		dlightMesh = MakeTetrahedron(description, 2)->MakeInstance();
		cameraMesh = MakeTetrahedron(description, 1)->MakeInstance();

		plightMesh.Data()->Initialize(Renderer->Device);
		dlightMesh.Data()->Initialize(Renderer->Device);
		cameraMesh.Data()->Initialize(Renderer->Device);
		
		plightMesh.SetMaterial(lightMaterial);
		dlightMesh.SetMaterial(lightMaterial);
		cameraMesh.SetMaterial(cameraMaterial);

		camera = new PerspectiveCamera(1.7f, 1.777f, 0.01f, 1000.0f);

		Entity cameraEntity = Space->CreateEntity<Transform, EditorCameraController>();

		Transform* transform = cameraEntity.SetComponent<Transform>(5);
		                       cameraEntity.SetComponent<EditorCameraController>(camera);

		camera->SetTrans(transform);

		cameraSystem = new EditorCameraControllerSystem();
		PushSystem<DrawCollidersSystem>(camera);



		// Particle test

		ref<Shader> particleShader = Asset->Load<Shader>("shaders/particle/simple.shader");
		Renderer->InitShader(particleShader, CAMERA);

		MeshDescription particleDec;

		particleDec.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));
		//description.DescribeBuffer(bName::UV,       MakeLayout<float>(2));

		VertexBufferLayout instanceLayout(1);
		instanceLayout.Push<float>(4);
		instanceLayout.Push<float>(4);
		instanceLayout.Push<float>(4);
		instanceLayout.Push<float>(4);

		particleDec.DescribeBuffer(bName::UV1, instanceLayout);

		MeshData* meshdata = MakePlane(particleDec, 1, 1);

		matrix4 mats[4] = {
			matrix4::create_translation(0, 2, 0),
			matrix4::create_translation(2, 0, 0),
			matrix4::create_translation(-2, 0, 0),
			matrix4::create_translation(0, 2, 0),
		};

		meshdata->SetBufferData(bName::UV1, 4, mats);
		meshdata->Initialize(Renderer->Device);

		iw::Material particleMaterial(particleShader);
		particleMaterial.Set("color", Color::From255(180, 0, 20));
		particleMaterial.Initialize(Renderer->Device);

		particles = meshdata->MakeInstance();
		particles.SetMaterial(particleMaterial.MakeInstance());

		return Layer::Initialize();
	}

	void ToolLayer::OnPush() {
		oldcamera = scene->MainCamera();
		scene->SetMainCamera(camera);
	}

	void ToolLayer::OnPop() {
		scene->SetMainCamera(oldcamera);
	}

	vector3 p;

	void ToolLayer::PostUpdate() {
		//for (auto camera : Space->Query<Transform, CameraController>()) {
		//	auto [camTransform, camController] = camera.Components.Tie<CameraComponents>();

		//	Renderer->BeginScene(camController->Camera);

		//	for (auto entity : Space->Query<Transform, Mesh>()) {
		//		auto [transform] = entity.Components.Tie<ObjectComponents>();

		//		font->UpdateMesh(textMesh, std::to_string(entity.Index), .1f, 1);
		//		textMesh->Update(Renderer->Device);

		//		Transform world;
		//		world.Position = transform->WorldPosition();
		//		world.Scale    = transform->WorldScale();
		//		world.Rotation = transform->WorldRotation();

		//		//t.Rotation = iw::quaternion::from_look_at(t.Position, camera.FindComponent<Transform>()->Position);

		//		Renderer->DrawMesh(world, textMesh);
		//	}

		//	Renderer->EndScene();
		//}

		Renderer->BeginScene(camera);

			Transform transform;
			transform.Position = oldcamera->Position();
			transform.Scale = vector3(0.75f, 0.75f, 0.75f);

			Renderer->DrawMesh(transform, cameraMesh);

			for (PointLight* light : scene->PointLights()) {
				transform.Position = light->Position();
				transform.Scale    = 0.1f;

				Renderer->DrawMesh(transform, plightMesh);
			}

			for (DirectionalLight* light : scene->DirectionalLights()) {
				transform.Position = light->Position();
				transform.Rotation = light->Rotation();
				transform.Scale = 0.1f;

				Renderer->DrawMesh(transform, dlightMesh);
			}

		Renderer->EndScene();

		for (Light* light : scene->Lights()) {
			if (!light->CanCastShadows()) {
				continue;
			}

			Renderer->BeginShadowCast(light);

				for (auto entity : Space->Query<Transform, Model>()) {
					auto [transform, model] = entity.Components.Tie<ModelComponents>();

					for (Mesh& mesh : *model) {
						Renderer->DrawMesh(*transform, mesh);
					}
				}

			Renderer->EndShadowCast();
		}

		Renderer->BeginScene(scene);

			for (auto entity : Space->Query<Transform, Model>()) {
				auto [transform, model] = entity.Components.Tie<ModelComponents>();
				
				for (Mesh& mesh : *model) {
					Renderer->DrawMesh(*transform, mesh);
				}
			}

			Renderer->DrawMesh(Transform(vector3(0, 5, 0)), particles);

		Renderer->EndScene();

		Renderer->BeginScene(camera);

			Renderer->DrawMesh(p, plightMesh);

		Renderer->EndScene();


		//Renderer->BeginScene(scene->MainCamera());
		//	Renderer->DrawMesh(&textTransform, textMesh);
		//Renderer->EndScene();
	}

	void ToolLayer::ImGui() {
		ImGui::Begin("Toolbox");

		int draws = 0;
		for (auto entity : Space->Query<Transform, Mesh>()) {
			draws++;
		}

		ImGui::Value("Number of object", draws);

		for (PointLight* light : scene->PointLights()) {
			vector3 pos = light->Position();
			ImGui::SliderFloat3("Light pos", (float*)&pos, -25, 25);
			light->SetPosition(pos);

			float rad = light->Radius();
			ImGui::SliderFloat("Light rad", (float*)&rad, 0, 100);
			light->SetRadius(rad);
		}

		for (DirectionalLight* light : scene->DirectionalLights()) {
			quaternion rot = light->Rotation();
			ImGui::SliderFloat4("Light rot", (float*)&rot, 0, 1);
			light->SetRotation(rot);
		}

		ImGui::End();
	}

	bool ToolLayer::On(
		MouseButtonEvent& e)
	{
		if (   e.State
			&& e.Button == iw::LMOUSE)
		{
			matrix4 invview = camera->View()      .inverted();
			matrix4 invproj = camera->Projection().inverted();

			float x =  (e.InputStates[MOUSEX] / Renderer->Width()  - 0.5f) * 2;
			float y = -(e.InputStates[MOUSEY] / Renderer->Height() - 0.5f) * 2;

			vector4 clip = vector4(     x,      y, -1, 1) * invproj;
			vector4 eye  = vector4(clip.x, clip.y, -1, 0) * invview;

			vector3 ray = eye.xyz().normalized();

			p = camera->Position() + ray * 5;

			//for (auto entity : Space->Query<Transform>()) {
			//	Transform* transform = entity.Components.Tie<Transform*>();


			//	

			//}

		}

		if (e.Button == LMOUSE) {
			if (e.State) {
				PushSystem(cameraSystem);
			}

			else {
				PopSystem(cameraSystem);
			}
		}


		return Layer::On(e);
	}
}

