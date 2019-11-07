#include "iw/graphics/Renderer.h"
#include "iw/util/io/File.h"

namespace IW {
	Renderer::Renderer(
		const iwu::ref<IW::IDevice>& device)
		: Device(device)
		, m_camera(nullptr)
	{
		iwm::vector3 pos[4] = {
			iwm::vector3(-1,  1, 0),
			iwm::vector3(-1, -1, 0),
			iwm::vector3( 1, -1, 0),
			iwm::vector3( 1,  1, 0),
		};

		iwm::vector2 uvs[4] = {
			iwm::vector2(1, 0),
			iwm::vector2(1, 1),
			iwm::vector2(0, 1),
			iwm::vector2(0, 0)
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

	iwu::ref<IW::IPipeline>& Renderer::CreatePipeline(
		const char* vertex, 
		const char* fragment)
	{
		IW::IVertexShader*   vs = Device->CreateVertexShader(iwu::ReadFile(vertex).c_str());
		IW::IFragmentShader* fs = Device->CreateFragmentShader(iwu::ReadFile(fragment).c_str());
		IW::IPipeline* pipeline = Device->CreatePipeline(vs, fs);

		pipeline->SetBuffer("Camera", m_camera);

		return m_pipelines.emplace_back(pipeline);
	}

	//iwu::ref<IW::Mesh> Renderer::CreateMesh(
	//	const IW::MeshData& data,
	//	iwu::ref<IW::Material>& material)
	//{
	//	IW::Mesh* mesh = new IW::Mesh();
	//	mesh->SetMaterial(material);

	//	if (data.Vertices) {
	//		mesh->SetVertices(data.VertexCount, data.Vertices);
	//	}

	//	if (data.Normals) {
	//		mesh->SetNormals(data.VertexCount, data.Normals);
	//	}

	//	// color
	//	// normal
	//	
	//	mesh->SetIndices(data.FaceCount, data.Faces);
	//	mesh->Compile(Device);

	//	return iwu::ref<IW::Mesh>(mesh);
	//}

	void Renderer::Initialize() {
		m_camera = Device->CreateUniformBuffer(2 * sizeof(iwm::matrix4));
		m_filterMesh->Initialize(Device);
	}

	void Renderer::Begin() {
		Device->Clear();
	}

	void Renderer::End() {

	}

	void Renderer::BeginScene(
		IW::Camera* camera,
		IW::RenderTarget* target)
	{
		iwm::matrix4 buf[2] = {
			camera->GetProjection(),
			camera->GetView()
		};

		Device->UpdateUniformBufferData(m_camera, buf);

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
		const IW::Transform* transform, 
		const IW::Mesh* mesh)
	{
		const auto& material = mesh->Material;
		
		if(!material) {
			// draw with default material
			return;
		}

		IW::IPipeline* pipeline = &*mesh->Material->Pipeline;

		mesh->Material->Use(Device);

		pipeline->GetParam("model")
			->SetAsMat4(transform->Transformation());

		mesh->Draw(Device);
	}

	void Renderer::ApplyFilter(
		iwu::ref<IW::IPipeline>& pipeline,
		IW::RenderTarget* source,
		IW::RenderTarget* dest)
	{
		if (source == dest) return;
		
		IW::OrthographicCamera cam;
		BeginScene(&cam, dest);

		Device->SetPipeline(pipeline.get());

		pipeline->GetParam("filterTexture")->SetAsTexture(
			source->Textures[0].Handle);

		m_filterMesh->Draw(Device);

		EndScene();
	}
}
