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
		Renderer->InitShader(fontShader, CAMERA);

		iw::ref<Material> textMat = REF<Material>(fontShader);

		textMat->Set("color", iw::vector3(1, .25f, 1));
		textMat->SetTexture("fontMap", font->GetTexture(0));

		textMesh->SetMaterial(textMat);
		textMesh->Initialize(Renderer->Device);

		// Meshes

		ref<Shader> shader = Asset->Load<Shader>("shaders/debug/wireframe.shader");

		Renderer->InitShader(shader, CAMERA);

		ref<Material> lightMaterial = REF<Material>();
		lightMaterial->Set("color", iw::Color(1, 1, 0, 1));
		lightMaterial->SetWireframe(true);
		lightMaterial->SetShader(shader);

		plightMesh = MakeIcosphere(2);
		plightMesh->SetNormals(0, nullptr);
		plightMesh->SetMaterial(lightMaterial);
		plightMesh->Initialize(Renderer->Device);

		dlightMesh = MakeTetrahedron(2);
		dlightMesh->SetNormals(0, nullptr);
		dlightMesh->SetMaterial(lightMaterial);
		dlightMesh->Initialize(Renderer->Device);

		ref<Material> cameraMaterial = REF<Material>();
		cameraMaterial->Set("color", iw::Color(0.1f, 0, 1, 1));
		cameraMaterial->SetWireframe(true);
		cameraMaterial->SetShader(shader);

		cameraMesh = MakeTetrahedron(1);
		cameraMesh->SetNormals(0, nullptr);
		cameraMesh->SetMaterial(cameraMaterial);
		cameraMesh->Initialize(Renderer->Device);

		camera = new PerspectiveCamera(1.7f, 1.777f, 0.01f, 1000.0f);

		Entity cameraEntity = Space->CreateEntity<Transform, EditorCameraController>();

		Transform* transform = cameraEntity.SetComponent<Transform>(5);
		                       cameraEntity.SetComponent<EditorCameraController>(camera);

		camera->SetTrans(transform);

		cameraSystem = new EditorCameraControllerSystem();
		PushSystem<DrawCollidersSystem>(camera);

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
					for (size_t i = 0; i < model->MeshCount; i++) {
						if (model->Meshes[i].Material->CastShadows()) {
							Renderer->DrawMesh(transform, &model->Meshes[i]);
						}
					}
				}

			Renderer->EndShadowCast();
		}

		Renderer->BeginScene(scene);

			for (auto entity : Space->Query<Transform, Model>()) {
				auto [transform, model] = entity.Components.Tie<ModelComponents>();
				for (size_t i = 0; i < model->MeshCount; i++) {
					Renderer->DrawMesh(transform, &model->Meshes[i]);
				}
			}

		Renderer->EndScene();

		Renderer->BeginScene(camera);

		Transform t;
		t.Position = p;
		Renderer->DrawMesh(t, plightMesh);

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

		if (e.Button == XMOUSE1) {
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

