#include "iw/engine/Systems/LightRenderSystem.h"

namespace iw {
namespace Engine {
	LightRenderSystem::LightRenderSystem(
		Scene* scene)
		: System<Transform, Model>("Light Render")
		, m_scene(scene)
	{}

	void LightRenderSystem::Update(
		EntityComponentArray& eca)
	{
		for (iw::Light* light : m_scene->Lights()) {
			if (!light->CanCastShadows()) {
				continue;
			}

			Renderer->BeginShadowCast(light);

			for (auto entity : Space->Query<iw::Transform, iw::Model>()) {
				auto [transform, model] = entity.Components.Tie<Components>();
				for (iw::Mesh& mesh : *model) {
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
