#include "iw/engine/Components/Model.h"

namespace IwEngine {
	Model::Model(
		ModelData* data, 
		IwRenderer::IDevice* device)
		: Data(data)
		, Meshes(new Mesh[data->MeshCount])
		, MeshCount(data->MeshCount)
	{
		for (size_t i = 0; i < MeshCount; i++) {
			auto vbl = new IwRenderer::VertexBufferLayout();
			vbl->Push<float>(3);
			vbl->Push<float>(3);

			auto vb = device->CreateVertexBuffer(
				data->Meshes[i].VertexCount * sizeof(IwGraphics::Vertex),
				data->Meshes[i].Vertices);

			auto va = device->CreateVertexArray(1, &vb, &vbl);

			auto ib = device->CreateIndexBuffer(
				data->Meshes[i].FaceCount,
				data->Meshes[i].Faces);

			Meshes[i] = { va, ib, data->Meshes[i].FaceCount };
		}
	}

	Model::Model(
		Model&& copy) noexcept
		: Meshes(copy.Meshes)
		, MeshCount(copy.MeshCount)
	{
		copy.Meshes    = nullptr;
		copy.MeshCount = 0;
	}

	Model::Model(
		const Model& copy)
		: Meshes(new Mesh[copy.MeshCount])
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
		Meshes    = copy.Meshes;
		MeshCount = copy.MeshCount;

		copy.Meshes    = nullptr;
		copy.MeshCount = 0;

		return *this;
	}

	Model& Model::operator=(
		const Model& copy)
	{
		Meshes    = new Mesh[copy.MeshCount];
		MeshCount = copy.MeshCount;
		for (std::size_t i = 0; i < MeshCount; i++) {
			Meshes[i] = copy.Meshes[i];
		}

		return *this;
	}
}
