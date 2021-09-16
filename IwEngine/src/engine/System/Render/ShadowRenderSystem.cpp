#include "iw/engine/Systems/Render/ShadowRenderSystem.h"

#include "iw/graphics/Model.h"
#include "iw/engine/Components/UiElement_temp.h"

namespace iw {
namespace Engine {
	ShadowRenderSystem::ShadowRenderSystem(
		Scene* scene)
		: SystemBase("Mesh Shadow Render")
		, m_scene(scene)
	{}

	int ShadowRenderSystem::Initialize() {
		m_models = Space->MakeQuery<Transform, Model>();
		m_meshes = Space->MakeQuery<Transform, Mesh>();

		m_models.SetNone({ Space->RegisterComponent<UiElement>() });
		m_meshes.SetNone({ Space->RegisterComponent<UiElement>() });

		return 0;
	}

	void ShadowRenderSystem::Update() {
		EntityComponentArray models = Space->Query(m_models);
		EntityComponentArray meshes = Space->Query(m_meshes);

		auto renderer = Renderer; // this is so stupid why would it think its a type before a variable name only in lambdas???

		for (Light* light : m_scene->Lights()) {
			if (   !light->CanCastShadows()
				/*|| !light->Outdated()*/)
			{
				continue;
			}

			renderer->BeginShadowCast(light);

			models.Each<Transform, Model>([&](
				EntityHandle entity,
				Transform* transform,
				Model* model)
			{
				for (size_t i = 0; i < model->MeshCount(); i++) {
					Mesh& mesh = model->GetMesh(i);
					Transform tran = model->GetTransform(i);

					if (!mesh.Material->CastShadows()) {
						continue;
					}

					tran.SetParent(transform, false);

					renderer->DrawMesh(tran, mesh);
				}
			});

			meshes.Each<Transform, Mesh>([&](
				EntityHandle entity,
				Transform* transform,
				Mesh* mesh)
			{
				if (!mesh->Material->CastShadows()) {
					return;
				}

				renderer->DrawMesh(transform, mesh);
			});

			renderer->EndShadowCast();
		}
	}
}
}
