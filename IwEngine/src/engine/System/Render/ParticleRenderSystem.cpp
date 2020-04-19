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
		Renderer->BeginScene(m_scene);
		
		for (auto entity : eca) {
			auto [transform, system] = entity.Components.Tie<Components>();

			system->SetCamera(m_scene->MainCamera());
			system->UpdateParticleMesh();

			Renderer->DrawMesh(transform, &system->GetParticleMesh());
		}
			
		Renderer->EndScene(); 
	}
}
}
