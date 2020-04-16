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

	DirectionalLight* Render::GetLight() {
		return in<DirectionalLight*>(1);
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
		DirectionalLight* light = GetLight();
		float ambiance = GetAmbiance();
		float gamma = GetGamma();

		for (auto c_e : space->Query<Transform, CameraController>()) {
			auto [c_t, c_c] = c_e.Components.Tie<CameraComponents>();

			renderer->BeginScene(c_c->Camera);

			// Alpha pass
			std::vector<Transform*> transs;
			std::vector<Mesh*>      meshes;

			for (auto m_e : space->Query<Transform, Model>()) {
				auto [m_t, m_m] = m_e.Components.Tie<ModelComponents>();

				for (Mesh& mesh : *m_m) {
					if (mesh.Material()->Get<Color>("albedo")->a != 1) {
						transs.push_back(m_t);
						meshes.push_back(&mesh);
						continue;
					}

					renderer->Device->SetPipeline(mesh.Material()->Shader->Handle());

					mesh.Material()->Shader->Handle()->GetParam("ambiance")
						->SetAsFloat(ambiance);

					renderer->DrawMesh(m_t, &mesh);
				}
			}

			// scuffed alpha pass

			for (size_t i = 0; i < transs.size(); i++) {
				Transform* m_t = transs[i];
				Mesh&     mesh = *meshes[i];

				renderer->Device->SetPipeline(mesh.Material()->Shader->Handle());

				mesh.Material()->Shader->Handle()->GetParam("ambiance")
					->SetAsFloat(ambiance);

				renderer->DrawMesh(m_t, &mesh);
			}

			renderer->EndScene();
		}
	};
}
