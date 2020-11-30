#include "iw/graphics/QueuedRenderer.h"
#include "iw/util/io/File.h"
#include <assert.h>

namespace iw {
namespace Graphics {
	Renderer::Renderer(
		const iw::ref<IDevice>& device)
		: Device(device)
		, m_camera(nullptr)
		, m_light(nullptr)
		, m_meshData(nullptr)
		, m_shader(nullptr)
		, m_material(nullptr)
		, m_target(nullptr)
		, m_ambiance(.03f)
		, m_state(RenderState::INVALID)
	{
		MeshDescription description;
		description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));
		description.DescribeBuffer(bName::UV,       MakeLayout<float>(2));

		MeshData* data = MakePlane(description, 1, 1);
		data->TransformMeshData(Transform(0, 1, quaternion::from_euler_angles(iw::Pi * 0.5f, 0, 0)));

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

	void Renderer::SetDefaultTarget(
		ref<RenderTarget> target)
	{
		m_defaultTarget = target;
	}

	void Renderer::Begin(
		float time)
	{
		m_time = time;

		Device->Clear(); // errors
		// clear queues
	}

	void Renderer::End() {
		m_camera   = nullptr;
		m_light    = nullptr;
		m_meshData = nullptr;
		m_shader   = nullptr;
		m_material = nullptr;
		m_target   = nullptr;
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
		const ref<RenderTarget>& target,
		bool clear)
	{
		Renderer::SetCamera(camera);
		Renderer::SetTarget(target);

		if (clear) {
			Device->Clear();
		}

		m_ambiance = 0.0f;

		m_state = RenderState::SCENE;
	}

	void Renderer::BeginScene(
		Scene* scene,
		const ref<RenderTarget>& target,
		bool clear)
	{
		Renderer::BeginScene(scene->MainCamera(), target, clear);

		Renderer::SetPointLights      (scene->PointLights());
		Renderer::SetDirectionalLights(scene->DirectionalLights());

		m_ambiance = scene->Ambiance();
	}

	void Renderer::BeginShadowCast(
		Light* light,
		bool useParticleShader,
		bool clear)
	{
		if (!light->CanCastShadows()) {
			LOG_WARNING << "Tried to begin shadow cast with light that cannot cast shadows!";
			return;
		}

		if (useParticleShader && !light->ParticleShadowShader()) {
			LOG_WARNING << "Tried to begin particle shadow cast with light that has no particle shader!";
			return;
		}

		m_light = light;

		Renderer::SetShader(useParticleShader ? light->ParticleShadowShader() : light->ShadowShader());
		
		if (light->ShadowTarget()) {
			Renderer::SetTarget(light->ShadowTarget());
		}

		light->SetupShadowCast(this, useParticleShader, clear);

		if (clear) {
			Device->Clear();
		}

		m_state = RenderState::SHADOW_MAP;
	}

	void Renderer::EndScene() {
		m_state = RenderState::INVALID;
	}

	void Renderer::EndShadowCast() {
		if (m_state != RenderState::SHADOW_MAP || !m_light) {
			LOG_WARNING << "Tried to end shadow cast that never began!";
			return;
		}

		m_light->EndShadowCast(this);
		Renderer::EndScene();
	}

	void Renderer::DrawMesh(
		const Transform* transform,
		Mesh* mesh)
	{
		if (m_state == RenderState::INVALID) {
			LOG_WARNING << "Tried to draw mesh to renderer while in an invalid state!";
			return;
		}

		if (!mesh->Data()) {
			LOG_WARNING << "Tried to draw mesh without data to renderer!";
			return;
		}

		if (!mesh->Material()) {
			LOG_WARNING << "Tried to draw mesh without a material to renderer!";
			//return; this is not critical
		}

		else if (!mesh->Material()->Shader) {
			LOG_WARNING << "Tried to draw mesh with a material that has no shader to renderer!";
			return;
		}

		Renderer::SetMesh(mesh);
		Renderer::SetMaterial(mesh->Material());

		if (!m_shader) {
			LOG_WARNING << "Tried to draw mesh without setting an active shader!";
			return;
		}

		IPipelineParam* model = m_shader->Handle()->GetParam("model");
		if (model) {
			model->SetAsMat4(transform->WorldTransformation());
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
		const ref<RenderTarget>& target,
		Camera* camera)
	{
		//if (source == destination) return; // this prob has to be here

		Renderer::BeginScene(camera, target);
		Renderer::SetShader(shader);

		if (source) {
			IPipelineParam* in = m_shader->Handle()->GetParam("inTexture");
			if (in) {
				in->SetAsTexture(source->Tex(0)->Handle());
			}
		}

		if (target) {
			IPipelineParam* out = m_shader->Handle()->GetParam("outTexture");
			if (out) {
				out->SetAsTexture(target->Tex(0)->Handle());
			}
		}

		Renderer::SetMesh(&m_quad);
		m_quad.Draw(Device);

		Renderer::EndScene();
	}

	void Renderer::SetTarget(
		const ref<RenderTarget>& target,
		bool useDefault)
	{
		//if (m_target != target) {
			if (target) {
				if (!target->IsInitialized()) {
					target->Initialize(Device);
				}

				Device->SetViewport(target->Width(), target->Height());
				Device->SetFrameBuffer(target->Handle());
			}

			else {
				if (   useDefault
					&& m_defaultTarget)
				{
					SetTarget(m_defaultTarget);
				}

				else {
					Device->SetViewport(m_width, m_height);
					Device->SetFrameBuffer(nullptr);
				}
			}

			m_target = target;
		//}
	}

	void Renderer::SetCamera(
		Camera* camera)
	{
		matrix4 v   = matrix4::identity;
		matrix4 p   = matrix4::identity;
		matrix4 vp  = matrix4::identity;
		vector3 pos = vector3::zero;

		if (camera) {
			v   = camera->View();
			p   = camera->Projection();
			vp  = camera->ViewProjection();
			pos = camera->WorldPosition();
		}

		if (   m_cameraData.CameraPos != pos	
			|| m_cameraData.ViewProj  != vp)
		{
			m_cameraData.View      = v;
			m_cameraData.Proj      = p;
			m_cameraData.ViewProj  = vp;
			m_cameraData.CameraPos = pos;
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

		else {
			if (m_meshData != mesh->Data()) {
				m_meshData = mesh->Data();
				
				if (!m_meshData->IsInitialized()) {
					m_meshData->Initialize(Device);
				}
				
				mesh->Bind(Device);
			}

			if (m_meshData->IsOutdated()) {
				m_meshData->Update(Device);
			}
		}
	}

	void Renderer::SetShader(
		const iw::ref<Shader>& shader)
	{
		//if (m_shader != shader) { // newed to reset tex unit count somehow before this is valid
			m_shader = shader;

			if (!m_shader->IsInitialized()) {
				m_shader->Initialize(Device);
			}

			m_shader->Use(Device);

			if (m_state == RenderState::SCENE) {
				IPipelineParam* ambiance = m_shader->Handle()->GetParam("ambiance"); // not safe \/
				if (ambiance) {
					ambiance->SetAsFloat(m_ambiance);
				}
			}

			else {
				Device->SetWireframe(false);  // set some shadow material or whatever
			}

			IPipelineParam* time = m_shader->Handle()->GetParam("time");
			if (time) {
				time->SetAsFloat(m_time);
			}

			if (m_debugState != -1) {
				IPipelineParam* state = m_shader->Handle()->GetParam("d_state");
				if (state) {
					state->SetAsInt(m_debugState);
				}
			}

		//}
	}

	void Renderer::SetMaterial(
		const ref<Material>& material)
	{
		if (material) {
			if (!material->IsInitialized()) {
				material->Initialize(Device);
			}

			if (m_state == RenderState::SCENE) {
				Renderer::SetShader(material->Shader);
			}
		}

		if (m_material != material) {
			if (material) {
				material->Use(Device, m_shader);
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
		m_lightData.LightCounts.x = lights.size();
		for (size_t i = 0; i < lights.size(); i++) {
			m_lightData.PointLights[i].Position   = lights[i]->WorldPosition();
			m_lightData.PointLights[i].Position.w = lights[i]->Radius();
			m_lightData.PointLights[i].Color      = lights[i]->Color();
		}

		Device->UpdateBuffer(m_lightUBO, &m_lightData);
	}

	void Renderer::SetDirectionalLights(
		const std::vector<DirectionalLight*>& lights)
	{
		m_lightData.LightCounts.y = lights.size();
		for (size_t i = 0; i < lights.size(); i++) {
			m_lightData.DirectionalLights[i].InvDirection = -vector3::unit_z * lights[i]->WorldRotation();
			m_lightData.DirectionalLights[i].Color        = lights[i]->Color();
		}

		m_shadowData.directionalLightCount.x = lights.size();
		for (size_t i = 0; i < lights.size(); i++) {
			m_shadowData.LightViewProj[i] = lights[i]->ViewProjection();
		}

		Device->UpdateBuffer(m_lightUBO,  &m_lightData);
		Device->UpdateBuffer(m_shadowUBO, &m_shadowData);
	}
}
}
