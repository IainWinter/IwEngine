#include "iw/engine/Systems/Render/MeshShadowRenderSystem.h"

namespace iw {
namespace Engine {
	MeshShadowRenderSystem::MeshShadowRenderSystem(
		Scene* scene)
		: System("Mesh Shadow Render")
		, m_scene(scene)
	{}

	void MeshShadowRenderSystem::Update(
		EntityComponentArray& eca)
	{
		for (iw::Light* light : m_scene->Lights()) {
			if (!light->CanCastShadows() || !light->Outdated()) {
				continue;
			}

			Renderer->BeginShadowCast(light);

			for (auto entity : eca) {
				auto [transform, mesh] = entity.Components.Tie<Components>();
				if (mesh->Material()->CastShadows()) {
					Renderer->DrawMesh(transform, mesh);
				}
			}

			Renderer->EndShadowCast();
		}
	}
}
}
