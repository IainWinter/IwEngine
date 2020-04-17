#include "iw/engine/Systems/MeshRenderSystem.h"

namespace iw {
namespace Engine {
	MeshRenderSystem::MeshRenderSystem(
		Scene* scene)
		: System<Transform, Mesh>("Mesh Render")
		, m_scene(scene)
	{}

	void MeshRenderSystem::Update(
		EntityComponentArray& eca)
	{
		Renderer->BeginScene(m_scene);
		
		for (auto entity : eca) {
			auto [transform, mesh] = entity.Components.Tie<Components>();
			Renderer->DrawMesh(transform, mesh);
		}
			
		Renderer->EndScene(); 
	}
}
}
