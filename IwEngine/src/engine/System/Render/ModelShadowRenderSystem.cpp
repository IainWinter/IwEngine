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
			if (!light->CanCastShadows()) {
				continue;
			}

			Renderer->BeginShadowCast(light);

			for (auto entity : eca) {
				auto [transform, model] = entity.Components.Tie<Components>();
				for (iw::Mesh& mesh : model->GetMeshes()) {
					if (mesh.Material()->CastShadows()) {
						Renderer->DrawMesh(*transform, mesh);
					}
				}
			}

			Renderer->EndShadowCast();
		}
	}
}
}
