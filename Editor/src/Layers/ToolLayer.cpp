#include "Layers/ToolLayer.h"
#include "Events/ActionEvents.h"
#include "Systems/EditorCameraController.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include "iw/graphics/Model.h"
#include "imgui/imgui.h"

namespace iw {
	struct ObjectComponents {
		Transform* Transform;
	};

	struct CameraComponents {
		Transform* Transform;
		CameraController* Controller;
	};

	ToolLayer::ToolLayer()
		 : Layer("Toolbox")
	{}

	int ToolLayer::Initialize() {
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

		PushSystem<EditorCameraController>();

		return Layer::Initialize();
	}

	void ToolLayer::PostUpdate() {
		for (auto camera : Space->Query<Transform, CameraController>()) {
			auto [camTransform, camController] = camera.Components.Tie<CameraComponents>();

			Renderer->BeginScene(camController->Camera);

			for (auto entity : Space->Query<Transform, Mesh>()) {
				auto [transform] = entity.Components.Tie<ObjectComponents>();

				font->UpdateMesh(textMesh, std::to_string(entity.Index), .1f, 1);
				textMesh->Update(Renderer->Device);

				Transform world;
				world.Position = transform->WorldPosition();
				world.Scale    = transform->WorldScale();
				world.Rotation = transform->WorldRotation();

				//t.Rotation = iw::quaternion::from_look_at(t.Position, camera.FindComponent<Transform>()->Position);

				Renderer->DrawMesh(world, textMesh);
			}

			Renderer->EndScene();
		}
	}

	void ToolLayer::ImGui() {
		ImGui::Begin("Toolbox");

		int draws = 0;
		for (auto entity : Space->Query<Transform, Mesh>()) {
			draws++;
		}

		ImGui::Value("Number of object", draws);

		ImGui::End();
	}

	bool ToolLayer::On(
		MouseButtonEvent& e)
	{
		//if (   e.State
		//	&& e.Button == iw::LMOUSE)
		//{
		//	matrix4 viewproj = (*Space->Query<Transform, CameraController>().begin()).Components.Tie<CameraComponents>().Controller->Camera->GetViewProjection(); // yak on em

		//	float x = (e.InputStates->GetState(MOUSEX) / Renderer->Width  - 0.5f) * 2;
		//	float y = (e.InputStates->GetState(MOUSEY) / Renderer->Height - 0.5f) * 2;

		//	vector4 begin(x, y, -1, 1);
		//	vector4 end  (x, y,  0, 1);

		//	vector4 wStart = begin * viewproj; wStart /= wStart.w;
		//	vector4 wEnd   = end   * viewproj; wEnd   /= wEnd  .w;

		//	LOG_INFO << (wEnd - wStart).normalized();
		//}

		return Layer::On(e);;
	}
}

