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
		for (iw::DirectionalLight* light : m_scene->DirectionalLights()) {
			if (!light->CanCastShadows()) {
				continue;
			}

			Renderer->BeginShadowCast(light, true, false);

			for (auto entity : eca) {
				auto [transform, system] = entity.Components.Tie<Components>();
				auto psystem = system; // not sure why it lamdas cant use the struct binding :c

				if (system->GetParticleMesh().Material()->CastShadows()) {
					Renderer->BeforeDraw([=]() {
						psystem->GetParticleMesh().Material()->Use(Renderer->Device);
						psystem->SetCamera(light->ShadowCamera());
						psystem->UpdateParticleMesh();
					});

					Renderer->DrawMesh(*transform, system->GetParticleMesh());
				}
			}

			Renderer->EndShadowCast();
		}

		//for (iw::PointLight* light : m_scene->PointLights()) {
		//	if (!light->CanCastShadows()) {
		//		continue;
		//	}

		//	Renderer->BeginShadowCast(light, true, false);

		//	for (auto entity : eca) {
		//		auto [transform, system] = entity.Components.Tie<Components>();
		//		auto psystem = system; // not sure why it lamdas cant use the struct binding :c

		//		if (system->GetParticleMesh().Material()->CastShadows()) {
		//			Renderer->BeforeDraw([=]() {
		//				psystem->SetCamera(nullptr);
		//				psystem->UpdateParticleMesh();
		//			});

		//			Renderer->DrawMesh(*transform, system->GetParticleMesh());
		//		}
		//	}

		//	Renderer->EndShadowCast();
		//}
	}
}
}
