#include "Layers/ToolLayer.h"
#include "imgui/imgui.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/MeshFactory.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include "iw/engine/Components/CameraController.h"
#include "Systems/EditorCameraController.h"

#include "Events/ActionEvents.h"

#include "iw/physics/Collision/PlaneCollider.h"

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
		Renderer->InitShader(shader, ALL);

		iw::ref<Material> smat = std::make_shared<Material>();
		smat->SetShader(shader);
		smat->SetFloats("albedo", &iw::vector4(1, .92f, 1, 1), 4);
		smat->SetTexture("albedoMap", nullptr);
		smat->SetTexture("ambientMap", nullptr);
		smat->SetTexture("displacementMap", nullptr);
		smat->SetTexture("normalMap", nullptr);

		iw::ref<Material> pmat = std::make_shared<Material>();
		pmat->SetShader(shader);
		pmat->SetFloats("albedo", &iw::vector4(1), 4);
		pmat->SetTexture("albedoMap", Asset->Load<Texture>("textures/moss/albedo.jpg"));
		pmat->SetTexture("ambientMap", Asset->Load<Texture>("textures/moss/ao.jpg"));
		pmat->SetTexture("displacementMap", Asset->Load<Texture>("textures/moss/displacement.jpg"));
		pmat->SetTexture("normalMap", Asset->Load<Texture>("textures/moss/normal.jpg"));

		Mesh* smesh = MakeUvSphere(25, 30);
		smesh->SetMaterial(smat);
		smesh->Initialize(Renderer->Device);
		
		Mesh* tmesh = MakeTetrahedron(5);
		tmesh->SetMaterial(smat);
		tmesh->Initialize(Renderer->Device);

		Mesh* pmesh = MakePlane(50, 50);
		pmesh->SetMaterial(pmat);
		pmesh->Initialize(Renderer->Device);

		Model pm { pmesh, 1 };

		iw::ref<Model> plane = Asset->Give<Model>("Floor", &pm);

		/*Asset->Load<Model>("models/grass.obj");*/

		for (size_t i = 0; i < plane->MeshCount; i++) {
			iw::ref<Material>& mat = plane->Meshes[i].Material;
			
			mat->SetShader(shader);
			mat->GetTexture("albedoMap")->Initialize(Renderer->Device);
			mat->GetTexture("ambientMap")->Initialize(Renderer->Device);
			mat->GetTexture("displacementMap")->Initialize(Renderer->Device);
			mat->GetTexture("normalMap") ->Initialize(Renderer->Device);

			//plane->Meshes[i].SetTangents(0, nullptr);
			//plane->Meshes[i].SetBiTangents(0, nullptr);

			plane->Meshes[i].Initialize(Renderer->Device);
		}

		Model sm { smesh, 1 };
		Model tm { tmesh, 1 };

		Asset->Give<Model>("Sphere", &sm);
		Asset->Give<Model>("Tetrahedron", &tm);

		PerspectiveCamera* perspective = new PerspectiveCamera(1.17f, 1.778f, .01f, 2000.0f);

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

