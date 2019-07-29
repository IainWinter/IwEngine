#include "iw/engine/Components/Model.h"

namespace IwEngine {
	Model::Model(
		ModelData* data, 
		IwRenderer::IDevice* device)
		: Meshes(new Mesh[data->MeshCount])
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
}
