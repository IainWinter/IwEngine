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
			Meshes[i] = RenderQueue.CreateMesh(
				meshData.VertexCount, 
				meshData.Vertices, 
				meshData.FaceCount, 
				meshData.Faces);
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
