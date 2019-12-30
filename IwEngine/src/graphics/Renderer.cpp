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
		m_lightBuf = Device->CreateUniformBuffer(sizeof(LightData), &m_lightData);
	}

	void Renderer::Begin() {
		//Device->Clear();
	}

	void Renderer::End() {

	}

	void Renderer::SetShader(
		const iw::ref<Shader>& shader)
	{
		Device->SetPipeline(shader->Program.get());
	}

	void Renderer::BeginScene(
		RenderTarget* target)
	{
		if (target == nullptr) {
			Device->SetViewport(Width, Height);
			Device->SetFrameBuffer(nullptr);
		}

		else {
			target->Use(Device);
		}

		Device->Clear();
	}

	void Renderer::EndScene() {

	}

	void Renderer::DrawMesh(
		const Transform* transform, 
		const Mesh* mesh)
	{
		const auto& material = mesh->Material;
		
		if(!material) {
			// draw with default material
			return;
		}

		IPipeline* pipeline = mesh->Material->Shader->Program.get();

		mesh->Material->Use(Device);

		pipeline->GetParam("model")
			->SetAsMat4(transform->Transformation());

		mesh->Draw(Device);
	}

	bool first = 1;

	void Renderer::BeginLight(
		Light* light)
	{
		BeginScene(light->LightTarget().get());
		SetShader (light->LightShader());

		m_lightData.ViewProj = light->ViewProj();
		Device->UpdateUniformBufferData(m_lightBuf, &m_lightData);

		if (first) {
			light->LightShader()->Program->SetBuffer("viewProjection", m_lightBuf);
			first = 0;
		}
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
		light->LightShader()->Program->GetParam("model")
			->SetAsMat4(transform->Transformation());

		mesh->Draw(Device);
	}

	void Renderer::ApplyFilter(
		iw::ref<Shader>& shader,
		RenderTarget* source,
		RenderTarget* dest)
	{
		if (source == dest) return;
		
		BeginScene(dest);

		SetShader(shader);

		shader->Program->GetParam("filterTexture")->SetAsTexture(
			source->Tex(0)->Handle());

		m_filterMesh->Draw(Device);

		EndScene();
	}
}
