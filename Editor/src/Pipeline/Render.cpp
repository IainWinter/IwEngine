#include "Pipeline/Render.h"

namespace iw {
	Render::Render(
		iw::ref<Renderer> r,
		iw::ref<Space> s)
		: iw::node(4, 0)
		, renderer(r)
		, space(s)
	{}

	void Render::SetLight(
		DirectionalLight& light)
	{
		set<DirectionalLight*>(1, &light);
	}

	void Render::SetAmbiance(
		float ambiance)
	{
		set<float>(2, ambiance);
	}

	void Render::SetGamma(
		float gamma)
	{
		set<float>(3, gamma);
	}

	iw::ref<Texture>& Render::GetTexture() {
		return in<iw::ref<Texture>>(0);
	}

	Light* Render::GetLight() {
		return in<Light*>(1);
	}

	float& Render::GetAmbiance() {
		return in<float>(2);
	}
	
	float& Render::GetGamma() {
		return in<float>(3);
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
		float ambiance = GetAmbiance();
		float gamma = GetGamma();

		for (auto c_e : space->Query<Transform, CameraController>()) {
			auto [c_t, c_c] = c_e.Components.Tie<CameraComponents>();

			c_c->Camera->Position = c_t->Position;
			c_c->Camera->Rotation = c_t->Rotation;

			renderer->BeginScene(c_c->Camera);

			// Alpha pass
			std::vector<Transform*> transs;
			std::vector<Mesh*>      meshes;

			for (auto m_e : space->Query<Transform, Model>()) {
				auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();

				for (size_t i = 0; i < m_m->MeshCount; i++) {
					Mesh& mesh = m_m->Meshes[i];

					if (mesh.Material->Get<Color>("albedo")->a != 1) {
						transs.push_back(m_t);
						meshes.push_back(&mesh);
						continue;
					}

					renderer->SetShader(mesh.Material->Shader);

					mesh.Material->Shader->Program->GetParam("lightSpace")
						->SetAsMat4(light->Cam().GetViewProjection());

					mesh.Material->Shader->Program->GetParam("ambiance")
						->SetAsFloat(ambiance);

					mesh.Material->Shader->Program->GetParam("gamma")
						->SetAsFloat(gamma);

					mesh.Material->Shader->Program->GetParam("sunPos")
						->SetAsFloats(&light->Cam().Position, 3);

					renderer->DrawMesh(m_t, &mesh);
				}
			}

			// scuffed alpha pass

			for (size_t i = 0; i < transs.size(); i++) {
				Transform* m_t = transs[i];
				Mesh&     mesh = *meshes[i];

				renderer->SetShader(mesh.Material->Shader);

				mesh.Material->Shader->Program->GetParam("lightSpace")
					->SetAsMat4(light->Cam().GetViewProjection());

				mesh.Material->Shader->Program->GetParam("ambiance")
					->SetAsFloat(ambiance);

				mesh.Material->Shader->Program->GetParam("gamma")
					->SetAsFloat(gamma);

				mesh.Material->Shader->Program->GetParam("sunPos")
					->SetAsFloats(&light->Cam().Position, 3);

				renderer->DrawMesh(m_t, &mesh);
			}

			renderer->EndScene();
		}
	};
}
