#include "iw/engine/Systems/Render/ModelVoxelRenderSystem.h"
#include "iw/graphics/VoxelLight.h"

namespace iw {
namespace Engine {
	ModelVoxelRenderSystem::ModelVoxelRenderSystem(
		Scene* scene)
		: System("Model Voxel Render")
		, m_scene(scene)
		, m_voxelize(nullptr)
		, m_visualize(false)
	{}

	int ModelVoxelRenderSystem::Initialize() {
		ref<Texture> voxelTexture = REF<Texture>(64, 64, TEX_3D, RGBA, UBYTE, BORDER);
		m_voxelize = new VoxelLight(voxelTexture, Asset->Load<Shader>("shaders/vct/voxelize.shader"));

		

		return 0;
	}

	void ModelVoxelRenderSystem::Update(
		EntityComponentArray& eca)
	{
		Renderer->BeginShadowCast(m_voxelize/*, false, false*/);

		for (auto entity : eca) {
			auto [transform, model] = entity.Components.Tie<Components>();
			for (iw::Mesh& mesh : model->GetMeshes()) {
				Renderer->DrawMesh(transform, &mesh);
			}
		}

		Renderer->EndShadowCast();

		if (!m_visualize) {
			return;
		}

		else if (!m_front) {
			ref<Texture> frntTex = REF<Texture>(Renderer->Width(), Renderer->Height(), TEX_2D, RGBA, UBYTE, BORDER);
			ref<Texture> backTex = REF<Texture>(Renderer->Width(), Renderer->Height(), TEX_2D, RGBA, UBYTE, BORDER);

			m_front = REF<RenderTarget>(Renderer->Width(), Renderer->Height());
			m_back  = REF<RenderTarget>(Renderer->Width(), Renderer->Height());

			m_front->AddTexture(frntTex);
			m_back ->AddTexture(backTex);

			MeshDescription description;
			description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));

			m_voxel = MakeCube(description)->MakeInstance();

			Material material;
			material.SetShader(Asset->Load<Shader>("shaders/vct/world_pos.shader"));
			
			m_voxel.SetMaterial(material.MakeInstance());
		}

		Renderer->BeginScene(m_scene->MainCamera(), m_front);

			Renderer->BeforeDraw([&]() {
				Renderer->Device->SetCullFace(CullFace::BACK);
			});

			Renderer->DrawMesh(Transform(), m_voxel);

		Renderer->EndScene();

		Renderer->BeginScene(m_scene->MainCamera(), m_back);

			Renderer->BeforeDraw([&]() {
				Renderer->Device->SetCullFace(CullFace::FRONT);
			});

			Renderer->DrawMesh(Transform(), m_voxel);
		
		Renderer->EndScene();
	}
}
}
