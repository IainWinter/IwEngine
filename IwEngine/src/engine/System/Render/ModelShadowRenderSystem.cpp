#include "iw/engine/Systems/Render/ModelShadowRenderSystem.h"

namespace iw {
namespace Engine {
	ModelShadowRenderSystem::ModelShadowRenderSystem(
		Scene* scene)
		: System("Model Shadow Render")
		, m_scene(scene)
	{}

	void ModelShadowRenderSystem::Update(
		EntityComponentArray& eca)
	{
		for (iw::Light* light : m_scene->Lights()) {
			if (   !light->CanCastShadows()
				/*|| !light->Outdated()*/)
			{
				continue;
			}

			Renderer->BeginShadowCast(light);

			for (auto entity : eca) {
				auto [transform, model] = entity.Components.Tie<Components>();

				for (int i = 0; i < model->MeshCount(); i++) {
					Mesh&     mesh = model->GetMesh(i);

					if (!mesh.Material()->CastShadows()) {
						continue;
					}

					Transform tran = model->GetTransform(i);
					tran.SetParent(transform, false);

					Renderer->DrawMesh(tran, mesh);
				}
			}

			Renderer->EndShadowCast();
		}
	}
}
}
