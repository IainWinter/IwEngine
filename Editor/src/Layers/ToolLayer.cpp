#include "Layers/ToolLayer.h"
#include "Systems/EditorCameraController.h"
#include "Events/ActionEvents.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "imgui/imgui.h"

namespace iw {
	struct ModelComponents {
		Transform* Transform;
		Model* Model;
	};

	struct CameraComponents {
		Transform* Transform;
		CameraController* Controller;
	};

	ToolLayer::ToolLayer()
		 : Layer("Tool")
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

		return 0;
	}

	void ToolLayer::PostUpdate() {
		for (auto camera : Space->Query<Transform, CameraController>()) {
			auto [camTransform, camController] = camera.Components.Tie<CameraComponents>();

			Renderer->SetCamera(camController->Camera);

			for (auto entity : Space->Query<Transform, Model>()) {
				auto [entTransform, entModel] = entity.Components.Tie<ModelComponents>();

				font->UpdateMesh(textMesh, std::to_string(entity.Index), .1f, 1);
				textMesh->Update(Renderer->Device);

				Transform t = *entTransform;
				t.Position.y += 1;
				t.Scale = 0.1f;
				t.Rotation = iw::quaternion::from_axis_angle(iw::vector3::unit_x, iw::Pi / 2);

				//t.Rotation = iw::quaternion::from_look_at(t.Position, camera.FindComponent<Transform>()->Position);

				Renderer->DrawMesh(&t, textMesh);
			}
		}
	}

	void ToolLayer::ImGui() {
		ImGui::Begin("Toolbox");

		if (ImGui::Button("Create Sphere")) {
			Bus->push<SingleEvent>(iw::val(Actions::SPAWN_CIRCLE_TEMP));
		}

		int draws = 0;
		for (auto entity : Space->Query<Transform, Model>()) {
			auto [t, m] = entity.Components.Tie<ModelComponents>();

			draws++;

			//std::stringstream s;
			//s << "Position ";
			//s << entity.Index;

			//ImGui::Value("Entity Index", (int)entity.Index);
			//ImGui::SliderFloat3(s.str().c_str(), (float*)&t->Position, -10, 10);
		}

		ImGui::Value("Number of object", draws);

		//for (auto entity : Space->Query<Transform, CameraController>()) {
		//	auto [t, m] = entity.Components.Tie<EditorCameraController::Components>();

		//	ImGui::SliderFloat3("Pos", (float*)&t->Position, -10, 10);
		//}

		ImGui::End();
	}
}

