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
		, m_width(0)
		, m_height(0)
		, m_time(0.f)
		, m_cameraUBO(nullptr)
		, m_shadowUBO(nullptr)
		, m_lightUBO(nullptr)
		, m_lightData()
		, m_shadowData()
		, m_cameraData()
	{}

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

	ref<Shader>& Renderer::InitShader(
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

		return shader;
	}

	void Renderer::BeginScene(
		Camera* camera,
		const ref<RenderTarget>& target,
		bool clear,
		Color clearColor)
	{
		Renderer::SetCamera(camera);
		Renderer::SetTarget(target);

		if (clear) {
			Renderer::SetClearColor(clearColor);
			Device->Clear();
		}

		m_ambiance = 0.0f;

		m_state = RenderState::SCENE;
	}

	void Renderer::BeginScene(
		Scene* scene,
		const ref<RenderTarget>& target,
		bool clear,
		Color clearColor)
	{
		Renderer::BeginScene(scene->MainCamera(), target, clear, clearColor);

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
			Device->Clear(); // set clear color?
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
		Transform* transform,
		Mesh* mesh)
	{
		if (m_state == RenderState::INVALID) {
			LOG_WARNING << "Tried to draw mesh to renderer while in an invalid state!";
			return;
		}

		if (!mesh->Data) {
			LOG_WARNING << "Tried to draw mesh without data to renderer!";
			return;
		}

		if (!mesh->Material) {
			LOG_WARNING << "Tried to draw mesh without a material to renderer!";
			//return; this is not critical
		}

		else if (!mesh->Material->Shader) {
			LOG_WARNING << "Tried to draw mesh with a material that has no shader to renderer!";
			return;
		}

		Renderer::SetMesh(mesh);
		Renderer::SetMaterial(mesh->Material);

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
		Transform& transform,
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

		Renderer::SetMesh(&ScreenQuad());
		ScreenQuad().Draw(Device);

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
					Device->SetViewport(m_width, m_height); // dont comment above
					Device->SetFrameBuffer(nullptr);
				}
			}

			m_target = target;
		//}
	}

	void Renderer::SetClearColor(
		Color color)
	{
		if (m_clearColor.to32() != color.to32()) { // male operator==
			m_clearColor = color;
		}

		Device->SetClearColor(
			m_clearColor.r, 
			m_clearColor.g, 
			m_clearColor.b, 
			m_clearColor.a
		);
	}

	void Renderer::SetCamera(
		Camera* camera)
	{
		glm::mat4 v   = glm::mat4(1);
		glm::mat4 p   = glm::mat4(1);
		glm::mat4 vp  = glm::mat4(1);
		glm::vec3 pos = glm::vec3();

		if (camera) {
			v   = camera->RecalculateView();
			p   = camera->Projection;
			vp  = camera->ViewProjection();
			pos = camera->Transform.WorldPosition();
		}

		if (   glm::vec3(m_cameraData.CameraPos) != pos	
			|| m_cameraData.ViewProj  != vp)
		{
			m_cameraData.View      = v;
			m_cameraData.Proj      = p;
			m_cameraData.ViewProj  = vp;
			m_cameraData.CameraPos = glm::vec4(pos, 0);
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
			if (m_meshData != mesh->Data) {
				m_meshData = mesh->Data;
				
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
		m_lightData.LightCounts.x = float(lights.size());
		for (size_t i = 0; i < lights.size(); i++) {
			m_lightData.PointLights[i].Position = glm::vec4(lights[i]->WorldPosition(), lights[i]->Radius());
			m_lightData.PointLights[i].Color    = glm::vec4(lights[i]->Color(), 1);
		}

		Device->UpdateBuffer(m_lightUBO, &m_lightData);
	}

	void Renderer::SetDirectionalLights(
		const std::vector<DirectionalLight*>& lights)
	{
		m_lightData.LightCounts.y = float(lights.size());
		for (size_t i = 0; i < lights.size(); i++) {
			m_lightData.DirectionalLights[i].InvDirection = glm::vec4(lights[i]->WorldRotation() * glm::vec3(0, 0, -1), 0);
			m_lightData.DirectionalLights[i].Color        = glm::vec4(lights[i]->Color(), 1);
		}

		m_shadowData.directionalLightCount.x = float(lights.size());
		for (size_t i = 0; i < lights.size(); i++) {
			m_shadowData.LightViewProj[i] = lights[i]->ViewProjection();
		}

		Device->UpdateBuffer(m_lightUBO,  &m_lightData);
		Device->UpdateBuffer(m_shadowUBO, &m_shadowData);
	}
}
}
