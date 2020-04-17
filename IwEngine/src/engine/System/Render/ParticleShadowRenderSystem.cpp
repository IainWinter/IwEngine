#include "iw/engine/Systems/Render/ParticleShadowRenderSystem.h"

namespace iw {
namespace Engine {
	ParticleShadowRenderSystem::ParticleShadowRenderSystem(
		Scene* scene)
		: System("Particle Shasdow Render")
		, m_scene(scene)
	{}

	void ParticleShadowRenderSystem::Update(
		EntityComponentArray& eca)
	{
		for (iw::Light* light : m_scene->Lights()) {
			if (!light->CanCastShadows()) {
				continue;
			}

			Renderer->BeginShadowCast(light, true, false);

			for (auto entity : eca) {
				auto [transform, system] = entity.Components.Tie<Components>();
				if (system->GetParticleMesh().Material()->CastShadows()) {
					Renderer->DrawMesh(*transform, system->GetParticleMesh());
				}
			}

			Renderer->EndShadowCast();
		}
	}
}
}
