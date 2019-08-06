#include "iw/engine/Components/Model.h"

namespace IwEngine {
	Model::Model(
		IwGraphics::ModelData* data, 
		IwGraphics::RenderQueue& RenderQueue)
		: Data(data)
		, Meshes(new IwGraphics::Mesh[data->MeshCount])
		, MeshCount(data->MeshCount)
		, Initialized(false)
	{
		for (size_t i = 0; i < MeshCount; i++) {
			IwGraphics::MeshData& meshData = data->Meshes[i];

			IwRenderer::VertexBufferLayout layouts[1];
			layouts[0].Push<float>(3);
			layouts[0].Push<float>(3);

			iwu::potential<IwRenderer::IVertexBuffer*> buffers[1];
			buffers[0] = RenderQueue.QueuedDevice.CreateVertexBuffer(meshData.VertexCount * sizeof(IwGraphics::Vertex), meshData.Vertices);

			auto pib = RenderQueue.QueuedDevice.CreateIndexBuffer(meshData.FaceCount, meshData.Faces);
			auto pva = RenderQueue.QueuedDevice.CreateVertexArray(1, buffers, layouts);

			Meshes[i] = IwGraphics::Mesh { pva, pib, meshData.FaceCount };
		}
	}

	Model::Model(
		Model&& copy) noexcept
		: Data(copy.Data)
		, Meshes(copy.Meshes)
		, MeshCount(copy.MeshCount)
	{
		copy.Data      = nullptr;
		copy.Meshes    = nullptr;
		copy.MeshCount = 0;
	}

	Model::Model(
		const Model& copy)
		: Data(copy.Data)
		, Meshes(new IwGraphics::Mesh[copy.MeshCount])
		, MeshCount(copy.MeshCount)
	{
		for (std::size_t i = 0; i < MeshCount; i++) {
			Meshes[i] = copy.Meshes[i];
		}
	}

	Model::~Model() {
		delete[] Meshes;
	}

	Model& Model::operator=(
		Model&& copy) noexcept
	{
		Data      = copy.Data;
		Meshes    = copy.Meshes;
		MeshCount = copy.MeshCount;

		copy.Data      = nullptr;
		copy.Meshes    = nullptr;
		copy.MeshCount = 0;

		return *this;
	}

	Model& Model::operator=(
		const Model& copy)
	{
		Data      = copy.Data;
		Meshes    = new IwGraphics::Mesh[copy.MeshCount];
		MeshCount = copy.MeshCount;

		for (std::size_t i = 0; i < MeshCount; i++) {
			Meshes[i] = copy.Meshes[i];
		}

		return *this;
	}
}
