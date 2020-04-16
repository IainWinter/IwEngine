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

		renderer->BeginShadowCast(light);

		for (auto m_e : space->Query<Transform, Model>()) {
			auto [m_t, m_m] = m_e.Components.Tie<Components>();

			for (Mesh& mesh : *m_m) {
				iw::ref<Texture> t = mesh.Material()->GetTexture("alphaMaskMap");

				ITexture* it = nullptr;
				if (t) {
					it = t->Handle();
				}

				light->ShadowShader()->Handle()->GetParam("hasAlphaMask")  ->SetAsFloat(it != nullptr);
				light->ShadowShader()->Handle()->GetParam("alphaMask")     ->SetAsTexture(it, 0);
				light->ShadowShader()->Handle()->GetParam("alphaThreshold")->SetAsFloat(threshold);

				renderer->DrawMesh(*m_t, mesh);
			}
		}

		renderer->EndShadowCast();

		out(0, light->ShadowTarget());
	}
}
