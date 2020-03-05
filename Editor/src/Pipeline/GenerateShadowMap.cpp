#include "Pipeline/GenerateShadowMap.h"

namespace iw {
	GenerateShadowMap::GenerateShadowMap(
		iw::ref<Renderer> r,
		iw::ref<Space> s)
		: iw::node(2, 1)
		, renderer(r)
		, space(s)
	{}

	void GenerateShadowMap::SetLight(
		DirectionalLight& light)
	{
		set<DirectionalLight*>(0, &light);
	}

	void GenerateShadowMap::SetThreshold(
		float threshold)
	{
		set<float>(1, threshold);
	}

	Light* GenerateShadowMap::GetLight() {
		return in<Light*>(0);
	}

	float& GenerateShadowMap::GetThreshold() {
		return in<float>(1);
	}

	struct Components {
		Transform* Transform;
		Model* Model;
	};

	void GenerateShadowMap::execute() {
		Light* light = GetLight();
		float& threshold = GetThreshold();

		renderer->BeginLight(light);

		for (auto m_e : space->Query<Transform, Model>()) {
			auto [m_t, m_m] = m_e.Components.Tie<Components>();

			for (size_t i = 0; i < m_m->MeshCount; i++) {
				iw::ref<Texture> t = m_m->Meshes[i].Material->GetTexture("alphaMaskMap");

				ITexture* it = nullptr;
				if (t) {
					it = t->Handle();
				}

				light->LightShader()->Program->GetParam("hasAlphaMask")  ->SetAsFloat(it != nullptr);
				light->LightShader()->Program->GetParam("alphaMask")     ->SetAsTexture(it, 0);
				light->LightShader()->Program->GetParam("alphaThreshold")->SetAsFloat(threshold);

				renderer->CastMesh(light, m_t, &m_m->Meshes[i]);
			}
		}

		renderer->EndLight(light);

		out(0, light->LightTarget());
	}
}
