#include "Layers/ToolLayer.h"
#include "imgui/imgui.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include "iw/engine/Components/CameraController.h"
#include "Systems/EditorCameraController.h"

#include "Events/ActionEvents.h"

namespace IW {
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
		iw::ref<Shader> shader = Asset->Load<Shader>("shaders/default.shader");
		shader->Initialize(Renderer->Device);

		iw::ref<Material> mat = std::make_shared<Material>();
		mat->SetShader(shader);

		Mesh* mesh = mesh_factory::create_uvsphere(25, 30);
		mesh->GenNormals();
		mesh->SetMaterial(mat);
		mesh->Initialize(Renderer->Device);

		Model m { mesh, 1 };

		iw::ref<Model> sphere = Asset->Give<Model>("Sphere", &m);

		PerspectiveCamera* perspective = new PerspectiveCamera(1.17f, 1.778f, .01f, 200.0f);

		IW::Entity camera = Space->CreateEntity<Transform, CameraController>();
		Space->SetComponentData<Transform>(camera, iw::vector3(0, 0, -5));
		Space->SetComponentData<CameraController>(camera, perspective);

		PushSystem<EditorCameraController>();

		return 0;
	}

	void ToolLayer::PostUpdate() {

	}

	void ToolLayer::ImGui() {
		ImGui::Begin("Toolbox");

		if (ImGui::Button("Create Sphere")) {
			Bus->push<SingleEvent>(iw::val(Actions::SPAWN_CIRCLE_TEMP));
		}

		for (auto entity : Space->Query<Transform, Model>()) {
			auto [t, m] = entity.Components.Tie<ModelComponents>();

			std::stringstream s;
			s << "Position ";
			s << entity.Index;

			ImGui::Value("Entity Index", (int)entity.Index);
			ImGui::SliderFloat3(s.str().c_str(), (float*)&t->Position, -10, 10);
		}

		for (auto entity : Space->Query<Transform, CameraController>()) {
			auto [t, m] = entity.Components.Tie<EditorCameraController::Components>();

			ImGui::SliderFloat3("Pos", (float*)&t->Position, -10, 10);
		}

		ImGui::End();
	}
}

