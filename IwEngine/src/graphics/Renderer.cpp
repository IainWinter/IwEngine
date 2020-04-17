#include "iw/graphics/QueuedRenderer.h"
#include "iw/util/io/File.h"
#include <assert.h>

namespace iw {
namespace Graphics {
	Renderer::Renderer(
		const iw::ref<IDevice>& device)
		: Device(device)
		, m_camera(nullptr)
		, m_meshData(nullptr)
	{
		MeshDescription description;
		description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));
		description.DescribeBuffer(bName::UV,       MakeLayout<float>(2));

		MeshData* data = MakePlane(description, 1, 1);

		m_quad = data->MakeInstance();
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

	void Renderer::SetWidth(
		int width)
	{
		m_width = width;
	}

	void Renderer::SetHeight(
		int height)
	{
		m_height = height;
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
		if (!shader->IsInitialized()) {
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
	}

	void Renderer::BeginScene(
		Camera* camera,
		const ref<RenderTarget>& target)
	{
		Renderer::SetCamera(camera);
		Renderer::SetTarget(target);

		m_state = RenderState::SCENE;
	}

	void Renderer::BeginScene(
		Scene* scene,
		const ref<RenderTarget>& target)
	{
		Renderer::BeginScene(scene->MainCamera(), target);

		Renderer::SetPointLights      (scene->PointLights());
		Renderer::SetDirectionalLights(scene->DirectionalLights());
	}

	void Renderer::BeginShadowCast(
		Light* light)
	{
#ifdef IW_DEBUG
		if (!light->CanCastShadows()) {
			LOG_WARNING << "Tried to begin shadow cast with light that cannot cast shadows!";
		}
#endif
		Renderer::SetShader(light->ShadowShader());
		Renderer::SetTarget(light->ShadowTarget());

		light->SetupShadowCast(this);

		Device->Clear();

		m_state = RenderState::SHADOW_MAP;
	}

	void Renderer::EndScene() {
		m_state = RenderState::INVALID;
	}

	void Renderer::EndShadowCast() {
		m_target->Tex(0)->Handle()->GenerateMipMaps();
		Renderer::EndScene();
	}

	void Renderer::DrawMesh(
		const Transform* transform,
		Mesh* mesh)
	{
#ifdef IW_DEBUG
		if (m_state == RenderState::INVALID) {
			LOG_WARNING << "Tried to submit mesh to renderer while in invalid state!";
		}
#endif
		if (m_state == RenderState::SCENE) {
			if (mesh->Material()) {
				if (!mesh->Material()->IsInitialized()) {
					mesh->Material()->Initialize(Device);
				}

				Renderer::SetMaterial(mesh->Material());
			}
		}

		else {
			Device->SetWireframe(false);
		}

		IPipelineParam* model = m_shader->Handle()->GetParam("model");
		if (model) {
			model->SetAsMat4(transform->WorldTransformation());
		}

		Renderer::SetMesh(mesh);

		if (!m_meshData->IsInitialized()) {
			m_meshData->Initialize(Device);
		}

		if (m_meshData->IsOutdated()) {
			m_meshData->Update(Device);
		}

		mesh->Draw(Device);
	}

	void Renderer::DrawMesh(
		const Transform& transform,
		Mesh& mesh)
	{
		Renderer::DrawMesh(&transform, &mesh);
	}

	void Renderer::ApplyFilter(
		iw::ref<Shader>& shader,
		const ref<RenderTarget>& source,
		const ref<RenderTarget>& target)
	{
		//if (source == destination) return; // this prob has to be here

		Renderer::BeginScene((Camera*)nullptr, target);
		
		Renderer::SetShader(shader);

		if (source) {
			shader->Handle()->GetParam("filterTexture")->SetAsTexture(
				source->Tex(0)->Handle()
			);
		}

		Renderer::SetMesh(&m_quad);
		m_quad.Draw(Device);

		Renderer::EndScene();
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

			m_target = target;
		}
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

		m_camera = camera;
	}

	void Renderer::SetMesh(
		Mesh* mesh)
	{
		if (!mesh) {
			m_meshData = nullptr;
		}

		else if (m_meshData != mesh->Data()) {
			m_meshData = mesh->Data();

			if (m_meshData) {
				mesh->Bind(Device);
			}
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
		if (material) {
			Renderer::SetShader(material->Shader);
		}

		if (m_material != material) {
			if (material) {
				material->Use(Device);
			}

			else {
				//SetShader(Material::Default.Shader);
				//Material::Default.Use(Device);
			}
		}

		m_material = material;
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
