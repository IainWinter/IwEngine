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

#include "iw/engine/Systems/ParticleUpdateSystem.h"
#include "iw/engine/Systems/Render/MeshRenderSystem.h"
#include "iw/engine/Systems/Render/ModelRenderSystem.h"
#include "iw/engine/Systems/Render/ParticleRenderSystem.h"
#include "iw/engine/Systems/Render/MeshShadowRenderSystem.h"
#include "iw/engine/Systems/Render/ModelShadowRenderSystem.h"
#include "iw/engine/Systems/Render/ParticleShadowRenderSystem.h"

#include "iw/engine/Systems/Render/ModelVoxelRenderSystem.h"

#include "iw/input/Devices/Mouse.h"
#include <iw\engine\Systems\PhysicsSystem.h>

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
		, m_mainScene(scene)
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

		plightMesh = MakeIcosphere  (description, 2)->MakeInstance();
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

		Transform* transform = cameraEntity.Set<Transform>(0);
		                       cameraEntity.Set<EditorCameraController>(camera);

		camera->SetTrans(transform);

		cameraSystem = new EditorCameraControllerSystem();
		PushSystem<DrawCollidersSystem>(camera);
		PushSystem<PhysicsSystem>();

		//PushSystem<iw::ModelVoxelRenderSystem>(m_mainScene);

		//PushSystem<iw::    MeshShadowRenderSystem>(m_mainScene);
		//PushSystem<iw::   ModelShadowRenderSystem>(m_mainScene);
		//PushSystem<iw::ParticleShadowRenderSystem>(m_mainScene);
		//PushSystem<iw::          MeshRenderSystem>(m_mainScene);
		PushSystem<iw::         ModelRenderSystem>(m_mainScene);
		//PushSystem<iw::      ParticleRenderSystem>(m_mainScene);


		return Layer::Initialize();
	}

	void ToolLayer::OnPush() {
		oldcamera = m_mainScene->MainCamera();
		m_mainScene->SetMainCamera(camera);
	}

	void ToolLayer::OnPop() {
		m_mainScene->SetMainCamera(oldcamera);
	}

	vector3 p;

	void ToolLayer::PostUpdate() {
		//for (auto camera : Space->Query<Transform, CameraController>()) {
		//	auto [camTransform, camController] = camera.Components.Tie<CameraComponents>();
		//
		//	Renderer->BeginScene(camController->Camera);
		//
		//	for (auto entity : Space->Query<Transform, Mesh>()) {
		//		auto [transform] = entity.Components.Tie<ObjectComponents>();
		//
		//		font->UpdateMesh(textMesh, std::to_string(entity.Index), .1f, 1);
		//		textMesh->Update(Renderer->Device);
		//
		//		Transform world;
		//		world.Position = transform->WorldPosition();
		//		world.Scale    = transform->WorldScale();
		//		world.Rotation = transform->WorldRotation();
		//
		//		//t.Rotation = iw::quaternion::from_look_at(t.Position, camera.Find<Transform>()->Position);
		//
		//		Renderer->DrawMesh(world, textMesh);
		//	}
		//
		//	Renderer->EndScene();
		//}

		//Renderer->BeginScene(camera);

		//	Transform transform;
		//	transform.Position = oldcamera->Position();
		//	transform.Scale = vector3(0.75f, 0.75f, 0.75f);

		//	Renderer->DrawMesh(transform, cameraMesh);

		//	for (PointLight* light : m_mainScene->PointLights()) {
		//		transform.Position = light->Position();
		//		transform.Scale    = 0.1f;

		//		Renderer->DrawMesh(transform, plightMesh);
		//	}

		//	for (DirectionalLight* light : m_mainScene->DirectionalLights()) {
		//		transform.Position = light->Position();
		//		transform.Rotation = light->Rotation();
		//		transform.Scale = 0.1f;

		//		Renderer->DrawMesh(transform, dlightMesh);
		//	}

		//Renderer->EndScene();

		//for (Light* light : m_mainScene->Lights()) {
		//	if (!light->CanCastShadows()) {
		//		continue;
		//	}

		//	Renderer->BeginShadowCast(light);

		//		for (auto entity : Space->Query<Transform, Model>()) {
		//			auto [transform, model] = entity.Components.Tie<ModelComponents>();

		//			for (Mesh& mesh : *model) {
		//				Renderer->DrawMesh(*transform, mesh);
		//			}
		//		}

		//	Renderer->EndShadowCast();
		//}

		//Renderer->BeginScene(m_mainScene);

		//	for (auto entity : Space->Query<Transform, Model>()) {
		//		auto [transform, model] = entity.Components.Tie<ModelComponents>();
		//		
		//		for (Mesh& mesh : *model) {
		//			Renderer->DrawMesh(*transform, mesh);
		//		}
		//	}

		//Renderer->EndScene();

		//Renderer->BeginScene(camera);

		//	Renderer->DrawMesh(p, plightMesh);

		//Renderer->EndScene();


		//Renderer->BeginScene(m_mainScene->MainCamera());
		//	Renderer->DrawMesh(&textTransform, textMesh);
		//Renderer->EndScene();
	}

	void ToolLayer::ImGui() {
		ImGui::Begin("Toolbox");

		float amb = m_mainScene->Ambiance();
		ImGui::SliderFloat("Ambiance", &amb, 0, 1);
		m_mainScene->SetAmbiance(amb);

		char name = 'A';

		for (PointLight* light : m_mainScene->PointLights()) {
			std::stringstream ss;
			ss << name << " Light pos";

			vector3 pos = light->Position();
			ImGui::SliderFloat3(ss.str().c_str(), (float*)&pos, 0, 10);
			light->SetPosition(pos);

			ss = std::stringstream();
			ss << name++ << " Light rad";

			float rad = light->Radius();
			ImGui::SliderFloat(ss.str().c_str(), &rad, 0, 100);
			light->SetRadius(rad);
		}

		for (DirectionalLight* light : m_mainScene->DirectionalLights()) {
			std::stringstream ss;
			ss << name << " Light pos";

			vector3 pos = light->Position();
			ImGui::SliderFloat3(ss.str().c_str(), (float*)&pos, 0, 10);
			light->SetPosition(pos);

			ss = std::stringstream();
			ss << name++ << " Light rot";

			vector3 rot = light->Rotation().euler_angles();
			ImGui::SliderFloat3(ss.str().c_str(), (float*)&rot, -Pi, Pi);
			light->SetRotation(quaternion::from_euler_angles(rot));
		}

		for (auto e : Space->Query<iw::Transform, iw::Model>()) {
			auto [t, m] = e.Components.Tie<ModelComponents>();

			iw::CollisionObject* o = Space->FindComponent<iw::CollisionObject>(e.Handle);

			std::stringstream ss;
			ss << e.Index << " pos";

			ImGui::SliderFloat3(ss.str().c_str(), (float*)&t->Position, -15, 15);

			ss = std::stringstream();
			ss << e.Index << " scale";

			ImGui::SliderFloat3(ss.str().c_str(), (float*)&t->Scale, 0, 15);

			ss = std::stringstream();
			ss << e.Index << " rotation";

			ImGui::SliderFloat4(ss.str().c_str(), (float*)&t->Rotation, -1, 1);
			t->Rotation.normalize();

			if(o) o->SetTrans(t);
		}

		ImGui::End();
	}

	bool ToolLayer::On(
		MouseButtonEvent& e)
	{
		if (   e.State
			&& e.Button == iw::LMOUSE
			&& e.Device == DeviceType::MOUSE)
		{
			// this works ezpz but not if yor looking in thr direction of the x axis :/

			//matrix4 invview = camera->View()      .inverted();
			//matrix4 invproj = camera->Projection().inverted();

			//float x =  (e.InputStates[MOUSEX] / Renderer->Width()  - 0.5f) * 2;
			//float y = -(e.InputStates[MOUSEY] / Renderer->Height() - 0.5f) * 2;

			//vector4 clip = vector4(     x,      y, -1, 1) * invproj;
			//vector4 eye  = vector4(clip.x, clip.y, -1, 0) * invview;

			//vector3 ray = eye.xyz().normalized();

			//p = camera->Position() + ray * 5;

			//for (auto entity : Space->Query<Transform>()) {
			//	Transform* transform = entity.Components.Tie<Transform*>();


			//	

			//}

		}

		if (   e.Button == LMOUSE
			&& e.Device == DeviceType::MOUSE)
		{
			if (e.State) {
				PushSystem(cameraSystem);
			}

			else {
				while (GetSystem<EditorCameraControllerSystem>("Editor Camera Controller")) {
					PopSystem(cameraSystem);
				}
			}
		}


		return Layer::On(e);
	}
}

