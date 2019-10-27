#include "iw/graphics/Renderer.h"
#include "iw/util/io/File.h"

namespace IW {
	Renderer::Renderer(
		const iwu::ref<IW::IDevice>& device)
		: Device(device)
		, m_camera(nullptr)
	{
	}

	iwu::ref<IW::IPipeline>& Renderer::CreatePipeline(
		const char* vertex, 
		const char* fragment)
	{
		IW::IVertexShader*   vs = Device->CreateVertexShader(iwu::ReadFile(vertex).c_str());
		IW::IFragmentShader* fs = Device->CreateFragmentShader(iwu::ReadFile(fragment).c_str());
		IW::IPipeline* pipeline = Device->CreatePipeline(vs, fs);

		if (!m_camera) {
			m_camera = Device->CreateUniformBuffer(2 * sizeof(iwm::matrix4));
		}

		pipeline->SetBuffer("Camera", m_camera);

		return m_pipelines.emplace_back(pipeline);
	}

	iwu::ref<IW::Mesh> Renderer::CreateMesh(
		const IW::MeshData& data,
		iwu::ref<IW::Material>& material)
	{
		IW::Mesh* mesh = new IW::Mesh();
		mesh->SetMaterial(material);

		if (data.Vertices) {
			mesh->SetVertices(data.VertexCount, data.Vertices);
		}

		if (data.Normals) {
			mesh->SetNormals(data.VertexCount, data.Normals);
		}

		// color
		// normal
		
		mesh->SetIndices(data.FaceCount, data.Faces);
		mesh->Compile(Device);

		return iwu::ref<IW::Mesh>(mesh);
	}

	void Renderer::Begin() {
		Device->Clear();
	}

	void Renderer::End() {

	}

	void Renderer::BeginScene(
		IW::Camera* camera)
	{
		iwm::matrix4 buf[2] = {
			camera->GetProjection(),
			camera->GetView()
		};

		Device->UpdateUniformBufferData(m_camera, buf);
	}

	void Renderer::EndScene() {

	}

	void Renderer::DrawMesh(
		const IW::Transform* transform, 
		const IW::Mesh* mesh)
	{
		IW::IPipeline* pipeline = &*mesh->Material->Pipeline;

		mesh->Material->Use(Device);

		pipeline->GetParam("model")
			->SetAsMat4(transform->Transformation());

		mesh->Draw(Device);
	}
}
