#include "iw/engine/Systems/Render/ModelVoxelRenderSystem.h"

#include "gl/glew.h"

namespace iw {
namespace Engine {
	ModelVoxelRenderSystem::ModelVoxelRenderSystem(
		Scene* scene)
		: System("Model Voxel Render")
		, m_scene(scene)
		, m_voxelize(nullptr)
		, m_visualize(true)
	{}

	int ModelVoxelRenderSystem::Initialize() {
		ref<Texture> voxelTexture = REF<Texture>(64, 64, TEX_3D, RGBA, UBYTE, BORDER);
		m_voxelize = new VoxelLight(m_scene->MainCamera(), voxelTexture, Asset->Load<Shader>("shaders/vct/voxelize.shader"));

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
			ref<Texture> backTex = REF<Texture>(Renderer->Width(), Renderer->Height(), TEX_2D, RGBA, UBYTE, BORDER);
			ref<Texture> frntTex = REF<Texture>(Renderer->Width(), Renderer->Height(), TEX_2D, RGBA, UBYTE, BORDER);

			m_back  = REF<RenderTarget>(Renderer->Width(), Renderer->Height());
			m_front = REF<RenderTarget>(Renderer->Width(), Renderer->Height());

			m_back ->AddTexture(backTex);
			m_front->AddTexture(frntTex);

			// Voxel

			Material vmaterial;
			vmaterial.SetShader(Asset->Load<Shader>("shaders/vct/world_pos.shader"));

			MeshDescription description;
			description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));

			m_bounds = MakeCube(description)->MakeInstance();			
			m_bounds.SetMaterial(vmaterial.MakeInstance());

			m_bounds.Data()->TransformMeshData(Transform(0, 1));

			// Quad

			Material qmaterial;
			qmaterial.SetShader(Asset->Load<Shader>("shaders/vct/visualize.shader"));
			qmaterial.SetTexture("back",  backTex);
			qmaterial.SetTexture("front", frntTex);
			qmaterial.SetTexture("world", m_voxelize->VoxelTexture());
			qmaterial.Set("level", 0.0f);

			m_quad = Renderer->ScreenQuad();
			m_quad.SetMaterial(qmaterial.MakeInstance());
		}

		// Back

		Renderer->BeginScene(m_scene->MainCamera(), m_back, true);

			Renderer->BeforeDraw([&]() {
				glClearColor(0.0, 0.0, 0.0, 1.0);
				glEnable(GL_CULL_FACE);
				glEnable(GL_DEPTH_TEST);
				glCullFace(GL_FRONT);
			});

			Renderer->DrawMesh(Transform(), m_bounds);

		Renderer->EndScene();

		// Front

		Renderer->BeginScene(m_scene->MainCamera(), m_front, true);

			Renderer->BeforeDraw([&]() {
				glCullFace(GL_BACK);
				glEnable(GL_DEPTH_TEST);
			});

			Renderer->DrawMesh(Transform(), m_bounds);

		Renderer->EndScene();

		// Visuals

		Renderer->BeginScene(m_scene->MainCamera(), nullptr, true);

			Renderer->BeforeDraw([&]() { glDisable(GL_DEPTH_TEST); });
			Renderer->AfterDraw ([&]() { glEnable (GL_DEPTH_TEST); });

			Renderer->DrawMesh(Transform(), m_quad);
		
		Renderer->EndScene();
	}
}
}
