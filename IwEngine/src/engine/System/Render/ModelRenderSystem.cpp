#include "iw/engine/Systems/Render/ModelRenderSystem.h"

namespace iw {
namespace Engine {
	ModelRenderSystem::ModelRenderSystem(
		Scene* scene)
		: System("Model Render")
		, m_scene(scene)
	{}

	void ModelRenderSystem::Update(
		EntityComponentArray& eca)
	{
		Renderer->BeginScene(m_scene);
		
		for (auto entity : eca) {
			auto [transform, model] = entity.Components.Tie<Components>();
			for (iw::Mesh& mesh : model->GetMeshes()) {
				Renderer->DrawMesh(transform, &mesh);
			}
		}

		Renderer->EndScene();
	}
}
}
