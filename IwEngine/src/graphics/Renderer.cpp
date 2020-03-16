#include "iw/graphics/QueuedRenderer.h"
#include "iw/util/io/File.h"
#include <assert.h>

namespace iw {
namespace Graphics {
	Renderer::Renderer(
		const iw::ref<IDevice>& device)
		: Device(device)
	{
		iw::vector3 pos[4] = {
			iw::vector3(-1,  1, 0),
			iw::vector3(-1, -1, 0),
			iw::vector3( 1,  1, 0),
			iw::vector3( 1, -1, 0),
		};

		iw::vector2 uvs[4] = {
			iw::vector2(0, 1),
			iw::vector2(0, 0),
			iw::vector2(1, 1),
			iw::vector2(1, 0)
		};

		unsigned int tris[6] = {
			0, 1, 2,
			1, 3, 2
		};

		Mesh* filterMesh = new Mesh();
		filterMesh->SetVertices(4, pos);
		filterMesh->SetUVs(4, uvs);
		filterMesh->SetIndices(6, tris);

		// needs to be initialized

		m_filterMesh = filterMesh;
	}

	Renderer::~Renderer() {
		delete m_filterMesh;
	}

	void Renderer::Initialize() {
		m_cameraUBO = Device->CreateUniformBuffer(&m_cameraData, sizeof(CameraData));
		m_shadowUBO = Device->CreateUniformBuffer(&m_shadowData, sizeof(ShadowData));
		m_lightUBO  = Device->CreateUniformBuffer(&m_lightData,  sizeof(LightData));
	}

	int Renderer::Width() const {
		return m_width;
	}

	int Renderer::Height() const {
		return m_height;
	}

	void Renderer::Resize(
		int width,
		int height)
	{
		m_width = width;
		m_height = height;
		Device->SetViewport(m_width, m_height);
	}

	void Renderer::Begin() {
		Device->Clear();

		// clear queues
	}

	void Renderer::End() {
		// nothing
	}

	void Renderer::InitShader(
		iw::ref<Shader>& shader,
		int bindings)
	{
		shader->Initialize(Device);
		
		if (bindings & CAMERA) {
			shader->Handle()->SetBuffer("Camera", m_cameraUBO);
		}

		if (bindings & SHADOWS) {
			shader->Handle()->SetBuffer("Shadows", m_shadowUBO);
		}

		if (bindings & LIGHTS) {
			shader->Handle()->SetBuffer("Lights", m_lightUBO);
		}

		//if (bindings & MATERIAL) {
		//	shader->Program->SetBuffer("Material", m_materialUBO);
		//}
	}

	void Renderer::BeginScene(
		Camera* camera,
		const ref<RenderTarget>& target)
	{
		SetCamera(camera);
		SetTarget(target);

		m_state = RenderState::SCENE;
	}

	void Renderer::BeginScene(
		Scene* scene,
		const ref<RenderTarget>& target)
	{
		Renderer::BeginScene(scene->Camera, target);

		SetPointLights      (scene->PointLights);
		SetDirectionalLights(scene->DirectionalLights);
	}

	void Renderer::BeginShadowCast(
		Light* light)
	{
#ifdef IW_DEBUG
		if (!light->CanCastShadows()) {
			LOG_WARNING << "Tried to begin shadow cast with light that cannot cast shadows!";
		}
#endif
		SetShader(light->ShadowShader());
		SetTarget(light->ShadowTarget());

		light->SetupShadowCast(this);

		Device->Clear();

		m_state = RenderState::SHADOW_MAP;
	}

	void Renderer::EndScene() {
		m_state = RenderState::INVALID;
	}

	void Renderer::EndShadowCast() {
		m_target->Tex(0)->Handle()->GenerateMipMaps();
		EndScene();
	}

	void Renderer::DrawMesh(
		const Transform* transform,
		const Mesh* mesh)
	{
#ifdef IW_DEBUG
		if (m_state == RenderState::INVALID) {
			LOG_WARNING << "Tried to submit mesh to renderer while in invalid state!";
		}
#endif
		if (m_state == RenderState::SCENE) {
			if (mesh->Material) {
				SetMaterial(mesh->Material);
			}
		}

		m_shader->Handle()->GetParam("model")->SetAsMat4(
			transform->Transformation()
		);

		mesh->Draw(Device);
	}

	void Renderer::ApplyFilter(
		iw::ref<Shader>& shader,
		const ref<RenderTarget>& source,
		const ref<RenderTarget>& destination)
	{
		if (source == destination) return;

		SetCamera(nullptr);
		SetShader(shader);
		SetTarget(destination);

		shader->Handle()->GetParam("filterTexture")->SetAsTexture(
			source->Tex(0)->Handle()
		);

		m_filterMesh->Draw(Device);

		EndScene();
	}

	void Renderer::SetTarget(
		const ref<RenderTarget>& target)
	{
		if (m_target != target) {
			if (target) {
				Device->SetViewport(target->Width(), target->Height());
				Device->SetFrameBuffer(target->Handle());
			}

			else {
				Device->SetViewport(m_width, m_height);
				Device->SetFrameBuffer(nullptr);
			}
		}

		m_target = target;
	}

	void Renderer::SetCamera(
		Camera* camera)
	{
		matrix4 vp  = matrix4::identity;
		vector3 pos = vector3::zero;

		if (camera) {
			vp  = camera->ViewProjection();
			pos = camera->Position();
		}

		if (   m_cameraData.CameraPos != pos	
			|| m_cameraData.ViewProj  != vp)
		{
			m_cameraData.CameraPos = pos;
			m_cameraData.ViewProj  = vp;
			Device->UpdateBuffer(m_cameraUBO, &m_cameraData);
		}
	}

	void Renderer::SetShader(
		const iw::ref<Shader>& shader)
	{
		//if (m_shader != shader) { // newed to reset tex unit count somehow before this is valid
			m_shader = shader;
			m_shader->Use(Device);
		//}
	}

	void Renderer::SetMaterial(
		const ref<Material>& material)
	{
		SetShader(material->Shader);
		
		if (m_material != material) {
			m_material = material;
			m_material->Use(Device);
		}
	}

	void Renderer::SetPointLights(
		const std::vector<PointLight*>& lights)
	{
		m_lightData.PointLightCount = lights.size();
		for (size_t i = 0; i < lights.size(); i++) {
			m_lightData.PointLights[i].Position = lights[i]->Position();
			m_lightData.PointLights[i].Radius   = lights[i]->Radius();
		}

		Device->UpdateBuffer(m_lightUBO, &m_lightData);
	}

	void Renderer::SetDirectionalLights(
		const std::vector<DirectionalLight*>& lights)
	{
		m_lightData.DirectionalLightCount = lights.size();
		for (size_t i = 0; i < lights.size(); i++) {
			m_lightData.DirectionalLights[i].InvDirection = -lights[i]->Rotation().euler_angles();
		}

		m_shadowData.DirectionalLightCount = lights.size();
		for (size_t i = 0; i < lights.size(); i++) {
			m_shadowData.LightViewProj[i] = lights[i]->ViewProjection();
		}

		Device->UpdateBuffer(m_lightUBO,  &m_lightData);
		Device->UpdateBuffer(m_shadowUBO, &m_shadowData);
	}
}
}
