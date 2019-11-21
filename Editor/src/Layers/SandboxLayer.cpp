#include "Layers/SandboxLayer.h"
#include "iw/graphics/Model.h"
#include "iw/engine/Components/CameraController.h"

namespace IW {
	struct ModelComponents {
		Transform* Transform;
		Model* Model;
	};

	struct CameraComponents {
		Transform* Transform;
		CameraController* Controller;
	};

	SandboxLayer::SandboxLayer()
		: Layer("Sandbox")
	{}

	void SandboxLayer::PostUpdate() {
		Renderer->BeginScene();

		for (auto c_e : Space->Query<Transform, CameraController>()) {
			auto [c_t, c_c] = c_e.Components.Tie<CameraComponents>();

			for (auto m_e : Space->Query<Transform, Model>()) {
				auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();

				for (size_t i = 0; i < m_m->MeshCount; i++) {
					Mesh& mesh = m_m->Meshes[i];

					mesh.Material->Shader->Program->GetParam("view")
						->SetAsMat4(c_c->Camera->GetView());

					mesh.Material->Shader->Program->GetParam("proj")
						->SetAsMat4(c_c->Camera->GetProjection());

					Renderer->DrawMesh(m_t, &mesh);
				}
			}
		}

		Renderer->EndScene();
	}
}

