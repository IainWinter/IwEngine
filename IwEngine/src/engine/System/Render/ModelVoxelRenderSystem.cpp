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

	float voxelSize    = 1.0f / (4.0f);
	float voxelSizeInv = 1.0f / voxelSize;

	vector3 voxelBoundsScale(32, 32, 32);
	vector3 voxelBoundsSize (256, 256, 256); // not squared :(
	vector3 voxelBoundsScaleInv = vector3(1) / voxelBoundsScale;

	int ModelVoxelRenderSystem::Initialize() {
		ref<Shader> vct = Asset->Load<Shader>("shaders/vct/vct.shader"); // Gets init from layer
		
		Renderer->SetShader(vct);
		vct->Handle()->GetParam("voxelBoundsScale")   ->SetAsFloats(&voxelBoundsScale, 3);
		vct->Handle()->GetParam("voxelBoundsScaleInv")->SetAsFloats(&voxelBoundsScaleInv, 3);
		vct->Handle()->GetParam("voxelSizeInv")       ->SetAsFloat (voxelSizeInv);
		vct->Handle()->GetParam("voxelSize")          ->SetAsFloat (voxelSize);

		ref<ComputeShader> mipmap = std::static_pointer_cast<ComputeShader>(Asset->Load<Shader>("shaders/vct/mipmap.shader"));

		ref<Shader> voxelize = Asset->Load<Shader>("shaders/vct/voxelize.shader");
		Renderer->InitShader(voxelize, LIGHTS | SHADOWS);

		Renderer->SetShader(voxelize);
		voxelize->Handle()->GetParam("voxelBoundsScale")   ->SetAsFloats(&voxelBoundsScale,    3);
		voxelize->Handle()->GetParam("voxelBoundsScaleInv")->SetAsFloats(&voxelBoundsScaleInv, 3);

		ref<Texture> voxelTexture = REF<Texture>(voxelBoundsSize.x, voxelBoundsSize.y, voxelBoundsSize.z, TEX_3D, RGBA, FLOAT, BORDER, LINEAR, LINEAR_LINEAR);
		m_voxelize = new VoxelLight(voxelTexture, voxelize, nullptr, mipmap);

		m_voxelize->SetPosition(vector3(0, 16, 0));
		
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
					if (mesh.Material()->IsInitialized()) {
						mesh.Material()->Initialize(Renderer->Device);
					}

					mesh.Material()->Use(Renderer->Device, m_voxelize->ShadowShader());

					IPipelineParam* ambianceParam = m_voxelize->ShadowShader()->Handle()->GetParam("ambiance");
					if (ambianceParam) ambianceParam->SetAsFloat (m_scene->Ambiance());

					float*       emissive  = mesh.Material()->Get<float>("emissive");
					ref<Texture> shadowMap = mesh.Material()->GetTexture("shadowMap");

					IPipelineParam* emissiveParam = m_voxelize->ShadowShader()->Handle()->GetParam("mat_emissive"); // to reset the emissive value in the shader :(
					if (emissiveParam) emissiveParam->SetAsFloat(emissive ? *emissive : 0);

					IPipelineParam* shadowMapParam = m_voxelize->ShadowShader()->Handle()->GetParam("mat_shadowMap"); // to reset the emissive value in the shader :(
					if (shadowMapParam) shadowMapParam->SetAsTexture(shadowMap ? shadowMap->Handle() : nullptr, 1);
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
			if (m_voxelize->VoxelTexture()->Filter() == NEAREST) {
				m_voxelize->VoxelTexture()->SetFilter(LINEAR);
				m_voxelize->VoxelTexture()->SetMipmapFilter(LINEAR_LINEAR);
			}

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

			m_bounds.Data()->TransformMeshData(Transform(0, voxelBoundsScale));

			// Quad

			ref<Shader> visualize = Asset->Load<Shader>("shaders/vct/visualize.shader");
			Renderer->InitShader(visualize, CAMERA);

			Renderer->SetShader(visualize);
			visualize->Handle()->GetParam("voxelBoundsScale")   ->SetAsFloats(&voxelBoundsScale, 3);
			visualize->Handle()->GetParam("voxelBoundsScaleInv")->SetAsFloats(&voxelBoundsScaleInv, 3);
			visualize->Handle()->GetParam("voxelSize")          ->SetAsFloat (voxelSize);

			Material qmaterial;
			qmaterial.SetShader(visualize);
			qmaterial.SetTexture("back",  backTex);
			qmaterial.SetTexture("front", frntTex);
			qmaterial.SetTexture("world", m_voxelize->VoxelTexture());
			qmaterial.Set("level", 0);

			m_quad = Renderer->ScreenQuad();
			m_quad.SetMaterial(qmaterial.MakeInstance());
		}

		if (m_voxelize->VoxelTexture()->Filter() == LINEAR) {
			m_voxelize->VoxelTexture()->SetFilter(NEAREST);
			m_voxelize->VoxelTexture()->SetMipmapFilter(NEAREST_NEAREST);
		}

		// Visuals

		// Back

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
