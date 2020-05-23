#include "iw/engine/Systems/Render/ModelVoxelRenderSystem.h"

#include "gl/glew.h"

#include "iw/engine/Time.h"

#include "iw/input/Devices/Keyboard.h"

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
		ref<Shader> voxelize = Asset->Load<Shader>("shaders/vct/voxelize.shader");

		Renderer->InitShader(voxelize, CAMERA | LIGHTS);

		// Filtering has a big effect on preformance (LINEAR is more expensive)
		ref<Texture> voxelTexture = REF<Texture>(64, 64, TEX_3D, RGBA, FLOAT, BORDER, LINEAR, LINEAR_LINEAR);
		m_voxelize = new VoxelLight(voxelTexture, voxelize);

		return 0;
	}

	//int i = 0;
	int d = 0;
	float t = 0.0f;

	void ModelVoxelRenderSystem::Update(
		EntityComponentArray& eca)
	{
		if (t > 0.001f && Keyboard::KeyDown(F)) {
			t = 0.0f;

			d = (d + 20) % 1000;
			Renderer->SetDebugState(d);
		}

		t += Time::DeltaTime();

		Renderer->BeginShadowCast(m_voxelize, false, false);

		for (auto entity : eca) {
			auto [transform, model] = entity.Components.Tie<Components>();
			
			vector3 scale    = transform->Scale;
			vector3 position = transform->Position;


			for (iw::Mesh& mesh : model->GetMeshes()) {
				if (!mesh.Material()->GetTexture("voxelMap")) {
					mesh.Material()->SetTexture("voxelMap", VoxelWorld());
				}

				Renderer->BeforeDraw([&]() {
					float* baseColor   = mesh.Material()->Get<float>("baseColor");
					float  reflectance = mesh.Material()->Get<float>("reflectance")[0];

					IPipeline* pipeline = m_voxelize->ShadowShader()->Handle();

					IPipelineParam* baseColorParam   = pipeline->GetParam("baseColor");
					IPipelineParam* reflectanceParam = pipeline->GetParam("reflectance");
					IPipelineParam* ambianceParam    = pipeline->GetParam("ambiance");

					if(baseColorParam)    baseColorParam  ->SetAsFloats(baseColor, 4);
					if (reflectanceParam) reflectanceParam->SetAsFloat (reflectance);
					if (ambianceParam)    ambianceParam   ->SetAsFloat (m_scene->Ambiance());
				});

				//if (i == 1) {
				//	transform->Scale *= 0.5f;
				//	transform->Position.x += randf() * 1.0f / 64.0f;
				//	transform->Position.y += randf() * 1.0f / 64.0f;
				//	transform->Position.z += randf() * 1.0f / 64.0f;
				//	i = 0;
				//}

				Renderer->DrawMesh(*transform, mesh);
			}

			transform->Scale    = scale;
			transform->Position = position;
		}

		Renderer->EndShadowCast();

		//i++;

		if (!m_visualize && !Keyboard::KeyDown(E)) {
			return;
		}

		else if (!m_front) {
			ref<Texture> backTex = REF<Texture>(Renderer->Width(), Renderer->Height(), TEX_2D, RGBA, FLOAT);
			ref<Texture> frntTex = REF<Texture>(Renderer->Width(), Renderer->Height(), TEX_2D, RGBA, FLOAT);

			m_back  = REF<RenderTarget>(Renderer->Width(), Renderer->Height());
			m_front = REF<RenderTarget>(Renderer->Width(), Renderer->Height());

			m_back ->AddTexture(backTex);
			m_front->AddTexture(frntTex);

			// Voxel

			ref<Shader> worldPos = Asset->Load<Shader>("shaders/vct/world_pos.shader");
			Renderer->InitShader(worldPos, CAMERA);

			Material vmaterial;
			vmaterial.SetShader(worldPos);

			MeshDescription description;
			description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));

			m_bounds = MakeCube(description)->MakeInstance();			
			m_bounds.SetMaterial(vmaterial.MakeInstance());

			m_bounds.Data()->TransformMeshData(Transform(0, 1));

			// Quad

			ref<Shader> visualize = Asset->Load<Shader>("shaders/vct/visualize.shader");
			Renderer->InitShader(visualize, CAMERA);

			Material qmaterial;
			qmaterial.SetShader(visualize);
			qmaterial.SetTexture("back",  backTex);
			qmaterial.SetTexture("front", frntTex);
			qmaterial.SetTexture("world", m_voxelize->VoxelTexture());
			qmaterial.Set("level", 0);

			m_quad = Renderer->ScreenQuad();
			m_quad.SetMaterial(qmaterial.MakeInstance());
		}

		Renderer->BeginScene(m_scene->MainCamera(), m_back, true);

			Renderer->BeforeDraw([&]() {
				//glClearColor(0.0, 0.0, 0.0, 1.0);
				glEnable(GL_CULL_FACE);
				glEnable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);
				glCullFace(GL_FRONT);
			});

			Renderer->DrawMesh(Transform(), m_bounds);

		Renderer->EndScene();

		// Front

		Renderer->BeginScene(m_scene->MainCamera(), m_front, true);

			Renderer->BeforeDraw([&]() {
				glCullFace(GL_BACK);
			});

			Renderer->DrawMesh(Transform(), m_bounds);

		Renderer->EndScene();

		// Visuals

		Renderer->BeginScene(m_scene->MainCamera(), nullptr);

			Renderer->BeforeDraw([&]() { glDisable(GL_DEPTH_TEST); });
			Renderer->AfterDraw ([&]() { glEnable (GL_DEPTH_TEST); });

			Renderer->DrawMesh(Transform(), m_quad);
		
		Renderer->EndScene();
	}

	ref<Texture> ModelVoxelRenderSystem::VoxelWorld() {
		return m_voxelize->VoxelTexture();
	}
}
}
