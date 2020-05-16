#include "iw/engine/Systems/Render/ModelVoxelRenderSystem.h"
#include "iw/graphics/VoxelLight.h"

namespace iw {
namespace Engine {
	ModelVoxelRenderSystem::ModelVoxelRenderSystem(
		Scene* scene)
		: System("Model Voxel Render")
		, m_scene(scene)
		, m_voxel(nullptr)
	{}

	int ModelVoxelRenderSystem::Initialize() {
		ref<Texture> voxelTexture = REF<Texture>(1024, 1024, TEX_3D, RGBA, UBYTE, BORDER);
		m_voxel = new VoxelLight(voxelTexture, Asset->Load<Shader>("shaders/vct/voxelize.shader"));

		return 0;
	}

	void ModelVoxelRenderSystem::Update(
		EntityComponentArray& eca)
	{
		Renderer->BeginShadowCast(m_voxel/*, false, false*/);

		for (auto entity : eca) {
			auto [transform, model] = entity.Components.Tie<Components>();
			for (iw::Mesh& mesh : model->GetMeshes()) {
				Renderer->DrawMesh(transform, &mesh);
			}
		}

		Renderer->EndShadowCast();
	}
}
}
