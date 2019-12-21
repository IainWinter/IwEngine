#include "iw/graphics/Renderer.h"
#include "iw/util/io/File.h"

namespace IW {
	Renderer::Renderer(
		const iw::ref<IDevice>& device)
		: Device(device)
	{
		iw::vector3 pos[4] = {
			iw::vector3(-1,  1, 0),
			iw::vector3(-1, -1, 0),
			iw::vector3( 1, -1, 0),
			iw::vector3( 1,  1, 0),
		};

		iw::vector2 uvs[4] = {
			iw::vector2(1, 0),
			iw::vector2(1, 1),
			iw::vector2(0, 1),
			iw::vector2(0, 0)
		};

		unsigned int tris[6] = {
			0, 1, 2,
			0, 2, 3
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
	}

	void Renderer::Begin() {
		//Device->Clear();
	}

	void Renderer::End() {

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

		IPipeline* pipeline = mesh->Material->Shader->Program;

		mesh->Material->Use(Device);

		pipeline->GetParam("model")
			->SetAsMat4(transform->Transformation());

		mesh->Draw(Device);
	}

	void Renderer::ApplyFilter(
		iw::ref<IPipeline>& pipeline,
		RenderTarget* source,
		RenderTarget* dest)
	{
		if (source == dest) return;
		
		BeginScene(dest);

		Device->SetPipeline(pipeline.get());

		pipeline->GetParam("filterTexture")->SetAsTexture(
			source->Textures[0].Handle);

		m_filterMesh->Draw(Device);

		EndScene();
	}
}
