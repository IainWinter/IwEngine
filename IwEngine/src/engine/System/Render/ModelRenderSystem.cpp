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

			for (int i = 0; i < model->MeshCount(); i++) {
				Mesh&     mesh = model->GetMesh(i);
				Transform tran = model->GetTransform(i);

				tran.SetParent(transform, false);

				Renderer->DrawMesh(tran, mesh);
			}
		}

		Renderer->EndScene();
	}
}
}
