#include "iw/engine/Systems/Render/ParticleRenderSystem.h"

namespace iw {
namespace Engine {
	ParticleRenderSystem::ParticleRenderSystem(
		Scene* scene)
		: System("Particle Render")
		, m_scene(scene)
	{}

	void ParticleRenderSystem::Update(
		EntityComponentArray& eca)
	{
		float ambiance = m_scene->Ambiance();
		Renderer->BeforeScene([&]() {
			m_scene->SetAmbiance(.5f); // for trees
		});

		Renderer->AfterScene([=]() {
			m_scene->SetAmbiance(ambiance);
		});

		Renderer->BeginScene(m_scene);

		for (auto entity : eca) {
			auto [transform, system] = entity.Components.Tie<Components>();
			auto psystem = system;

			Task->queue([=]() { // put in own system
				psystem->UpdateParticleMesh();
			});

			Renderer->DrawMesh(transform, &system->GetParticleMesh());
		}
			
		Renderer->EndScene();
	}
}
}
