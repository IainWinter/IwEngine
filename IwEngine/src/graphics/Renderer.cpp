#include "iw/graphics/Renderer.h"
#include "iw/util/io/File.h"
#include <assert.h>

namespace IW {
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

		m_filterMesh = new Mesh();
		m_filterMesh->SetVertices(4, pos);
		m_filterMesh->SetUVs(4, uvs);
		m_filterMesh->SetIndices(6, tris);
	}

	Renderer::~Renderer() {
		delete m_filterMesh;
	}

	void Renderer::Initialize() {
		m_filterMesh->Initialize(Device);
		m_cameraUBO   = Device->CreateUniformBuffer(sizeof(CameraData),   &m_cameraData);
		m_materialUBO = Device->CreateUniformBuffer(sizeof(MaterialData), &m_materialData);
	}

	void Renderer::Begin() {
		//Device->Clear();
	}

	void Renderer::End() {

	}

	void Renderer::InitShader(
		iw::ref<Shader>& shader,
		UBOBinding bindings)
	{
		shader->Initialize(Device);
		
		if (bindings & CAMERA) {
			shader->Program->SetBuffer("Camera", m_cameraUBO);
		}

		if (bindings & MATERIAL) {
			shader->Program->SetBuffer("Material", m_materialUBO);
		}
	}

	void Renderer::SetShader(
		const iw::ref<Shader>& shader)
	{
		Device->SetPipeline(shader->Program.get());
	}

	void Renderer::BeginScene(
		Camera* camera,
		RenderTarget* target)
	{
		if (target == nullptr) {
			Device->SetViewport(Width, Height);
			Device->SetFrameBuffer(nullptr);
		}

		else {
			target->Use(Device);
		}

		iw::matrix4 vp = iw::matrix4::identity;
		if (camera != nullptr) {
			vp = camera->GetViewProjection();
		}

		if (vp != m_cameraData.ViewProj) {
			m_cameraData.ViewProj = vp;
			Device->UpdateUniformBufferData(m_cameraUBO, &m_cameraData);
		}

		Device->Clear();
	}

	void Renderer::EndScene() {

	}

	void Renderer::DrawMesh(
		const Transform* transform, 
		const Mesh* mesh)
	{
		const iw::ref<Material>&  material = mesh->Material;
		const iw::ref<IPipeline>& pipeline = material->Shader->Program;
		
		if(!material) {
			// draw with default material
			return;
		}

		SetShader(material->Shader);

		//material->Use(Device);

		float* albedo      = std::get<0>(material->GetFloats("albedo"));
		Texture* albedoMap = material->GetTexture("albedoMap");

		if (albedo) {
			m_materialData.Albedo = *(iw::vector4*)albedo;
		}

		else {
			m_materialData.Albedo = iw::vector4::one;
		}

		m_materialData.HasAlbedoMap = !!albedoMap;
		if (albedoMap) {
			pipeline->GetParam("albedoMap")->SetAsTexture(albedoMap->Handle());
		}

		else {
			pipeline->GetParam("albedoMap")->SetAsTexture(nullptr);
		}

		Device->UpdateUniformBufferData(m_materialUBO, &m_materialData);

		pipeline->GetParam("model")->SetAsMat4(transform->Transformation());

		mesh->Draw(Device);
	}

	void Renderer::BeginLight(
		Light* light)
	{
		BeginScene(&light->Cam(), light->LightTarget().get());
		SetShader (light->LightShader());
	}

	void Renderer::EndLight(
		const Light* light)
	{
		// post process

		EndScene();
	}

	void Renderer::CastMesh(
		const Light* light, 
		const Transform* transform, 
		const Mesh* mesh)
	{
		mesh->Material->Shader->Program->GetParam("model")
			->SetAsMat4(transform->Transformation());

		mesh->Draw(Device);
	}

	void Renderer::ApplyFilter(
		iw::ref<Shader>& shader,
		RenderTarget* source,
		RenderTarget* dest)
	{
		if (source == dest) return;
		
		BeginScene(nullptr, dest);

		SetShader(shader);

		shader->Program->GetParam("filterTexture")->SetAsTexture(
			source->Tex(0)->Handle());

		m_filterMesh->Draw(Device);

		EndScene();
	}
}
