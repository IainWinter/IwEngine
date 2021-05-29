#include "iw/engine/Systems/Render/RenderSystem.h"

#include "iw/graphics/Model.h"
#include "iw/engine/Components/UiElement_temp.h"

namespace iw {
namespace Engine {
	RenderSystem::RenderSystem(
		Scene* scene,
		ref<RenderTarget> target,
		bool clear,
		Color clearColor
	)
		: SystemBase("Render")
		, m_scene(scene)
		, m_target(target)
		, m_clear(clear)
		, m_clearColor(clearColor)
	{}

	int RenderSystem::Initialize() {
		m_models = Space->MakeQuery<Transform, Model>();
		m_meshes = Space->MakeQuery<Transform, Mesh>();

		m_models->SetNone({ Space->RegisterComponent<UiElement>() });
		m_meshes->SetNone({ Space->RegisterComponent<UiElement>() });

		return 0;
	}

	void RenderSystem::Update() {
		EntityComponentArray models = Space->Query(m_models);
		EntityComponentArray meshes = Space->Query(m_meshes);

		auto renderer = Renderer; // this is so stupid why would it think its a type before a variable name only in lambdas???

		renderer->BeginScene(m_scene, m_target, m_clear, m_clearColor);

		models.Each<Transform, Model>([&](
			EntityHandle entity,
			Transform* transform,
			Model* model)
		{
			for (size_t i = 0; i < model->MeshCount(); i++) {
				Mesh& mesh = model->GetMesh(i);
				Transform tran = model->GetTransform(i);

				tran.SetParent(transform, false);

				renderer->DrawMesh(tran, mesh);
			}
		});

		meshes.Each<Transform, Mesh>([&](
			EntityHandle entity,
			Transform* transform,
			Mesh* mesh)
		{
			renderer->DrawMesh(transform, mesh);
		});

		renderer->EndScene();
	}
}
}
