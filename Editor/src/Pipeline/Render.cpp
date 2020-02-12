#include "Pipeline/Render.h"

namespace iw {
	Render::Render(
		iw::ref<Renderer> r,
		iw::ref<Space> s)
		: iw::node(2, 0)
		, renderer(r)
		, space(s)
	{}

	void Render::SetLight(
		DirectionalLight& light)
	{
		set<DirectionalLight*>(1, &light);
	}

	iw::ref<Texture>& Render::GetTexture() {
		return in<iw::ref<Texture>>(0);
	}

	Light* Render::GetLight() {
		return in<Light*>(1);
	}
	
	struct CameraComponents {
		Transform* Transform;
		CameraController* Camera;
	};

	struct ModelComponents {
		Transform* Transform;
		Model* Model;
	};

	void Render::execute() {
		iw::ref<Texture>& shadowMap = GetTexture();
		Light* light = GetLight();

		for (auto c_e : space->Query<Transform, CameraController>()) {
			auto [c_t, c_c] = c_e.Components.Tie<CameraComponents>();

			c_c->Camera->Position = c_t->Position;
			c_c->Camera->Rotation = c_t->Rotation;

			renderer->BeginScene(c_c->Camera);

			for (auto m_e : space->Query<Transform, Model>()) {
				auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();

				for (size_t i = 0; i < m_m->MeshCount; i++) {
					Mesh& mesh = m_m->Meshes[i];

					renderer->SetShader(mesh.Material->Shader);

					mesh.Material->Shader->Program->GetParam("lightSpace")
						->SetAsMat4(light->Cam().GetViewProjection());

					renderer->DrawMesh(m_t, &mesh);
				}
			}

			renderer->EndScene();
		}
	};
}
