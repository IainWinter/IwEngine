#include "iw/graphics/Renderer.h"

namespace IwGraphics {
	Renderer::Renderer(
		const iwu::ref<IwRenderer::IDevice>& device)
		: Device(device)
	{}

	Mesh* Renderer::CreateMesh(
		const MeshData& meshData)
	{
		IwRenderer::VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);

		size_t size = meshData.VertexCount * sizeof(IwGraphics::Vertex);

		IwRenderer::IIndexBuffer*  ib = Device->CreateIndexBuffer(meshData.FaceCount, meshData.Faces);
		IwRenderer::IVertexBuffer* vb = Device->CreateVertexBuffer(size, meshData.Vertices);
		
		IwRenderer::IVertexArray* va = Device->CreateVertexArray();
		va->AddBuffer(vb, layout);

		return new IwGraphics::Mesh { va, ib, meshData.FaceCount };
	}
}
