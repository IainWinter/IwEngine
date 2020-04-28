#include "iw/engine/Systems/Render/UiRenderSystem.h"

namespace iw {
namespace Engine {
	UiRenderSystem::UiRenderSystem(
		Camera* camera)
		: System<Transform, Mesh, UiElement>("Ui Render")
		, m_camera(camera)
	{}

	void UiRenderSystem::Update(
		EntityComponentArray& eca)
	{
		Renderer->BeginScene(m_camera);
		
		for (auto entity : eca) {
			auto [transform, mesh, element] = entity.Components.Tie<Components>();
			Renderer->DrawMesh(transform, mesh);
		}
			
		Renderer->EndScene(); 
	}
}
}
