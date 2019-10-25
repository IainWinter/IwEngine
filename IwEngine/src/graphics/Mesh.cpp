#pragma once

#include "iw/graphics/Mesh.h"

namespace IW {
	Mesh::Mesh()
		: Vertices(nullptr)
		, Normals(nullptr)
		, Colors(nullptr)
		, Uvs(nullptr)
		, Indices(nullptr)
		, VertexCount(0)
		, NormalCount(0)
		, ColorCount(0)
		, UvCount(0)
		, IndexCount(0)
		, Topology(IW::TRIANGLES)
		, VertexArray(nullptr)
		, IndexBuffer(nullptr)
		, Outdated(false)
	{}

	Mesh::Mesh(
		IW::MeshTopology topology)
		: Vertices(nullptr)
		, Normals(nullptr)
		, Colors(nullptr)
		, Uvs(nullptr)
		, Indices(nullptr)
		, VertexCount(0)
		, NormalCount(0)
		, ColorCount(0)
		, UvCount(0)
		, IndexCount(0)
		, Topology(topology)
		, VertexArray(nullptr)
		, IndexBuffer(nullptr)
		, Outdated(false)
	{}

	Mesh::~Mesh() {
		delete[] Vertices;
		delete[] Normals;
		delete[] Colors;
		delete[] Uvs;
		delete[] Indices;
	}

	void Mesh::Compile(
		const iwu::ref<IW::IDevice>& device)
	{
		size_t min = VertexCount + NormalCount + ColorCount + UvCount;
		if (VertexCount > 0 && VertexCount < min) min = VertexCount;
		if (NormalCount > 0 && NormalCount < min) min = NormalCount;
		if (ColorCount  > 0 && ColorCount  < min) min = ColorCount;
		if (UvCount     > 0 && UvCount     < min) min = UvCount;

		if (VertexCount > 0) VertexCount = min;
		if (NormalCount > 0) NormalCount = min;
		if (ColorCount  > 0) ColorCount  = min;
		if (UvCount     > 0) UvCount     = min;

		if (VertexArray && Outdated) { // reset data not sub data
			//int index = 0;
			//if (Vertices) {
			//	device->UpdateVertexArrayData(VertexArray, index, VertexCount * sizeof(iwm::vector3), Vertices);
			//	index++;
			//}

			//if (Normals) {
			//	device->UpdateVertexArrayData(VertexArray, index, NormalCount * sizeof(iwm::vector3), Normals);
			//	index++;
			//}

			//if (Colors) {
			//	device->UpdateVertexArrayData(VertexArray, index, ColorCount * sizeof(iwm::vector3), Colors);
			//	index++;
			//}

			//if (Uvs) {
			//	device->UpdateVertexArrayData(VertexArray, index, UvCount * sizeof(iwm::vector2), Uvs);
			//	index++;
			//}
		}

		else {
			IW::VertexBufferLayout layout3f;
			layout3f.Push<float>(3);

			IW::VertexBufferLayout layout2f;
			layout2f.Push<float>(3);

			VertexArray = device->CreateVertexArray();
			IndexBuffer = device->CreateIndexBuffer(IndexCount, Indices);
			
			if (Vertices) {
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(VertexCount * sizeof(iwm::vector3), Vertices);
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Normals) {
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(NormalCount * sizeof(iwm::vector3), Normals);
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Colors) {
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(ColorCount * sizeof(iwm::vector3), Colors);
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Uvs) {
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(UvCount * sizeof(iwm::vector2), Uvs);
				device->AddBufferToVertexArray(VertexArray, buffer, layout2f);
			}
		}

		Outdated = false;
	}

	void Mesh::Update(
		const iwu::ref<IW::IDevice>& device)
	{
		int index = 0;
		if (Vertices) {
			device->UpdateVertexArrayData(VertexArray, index, VertexCount * sizeof(iwm::vector3), Vertices);
			index++;
		}

		if (Normals) {
			device->UpdateVertexArrayData(VertexArray, index, NormalCount * sizeof(iwm::vector3), Normals);
			index++;
		}

		if (Colors) {
			device->UpdateVertexArrayData(VertexArray, index, ColorCount * sizeof(iwm::vector3), Colors);
			index++;
		}

		if (Uvs) {
			device->UpdateVertexArrayData(VertexArray, index, UvCount * sizeof(iwm::vector2), Uvs);
			index++;
		}
	}

	void Mesh::Destroy(
		const iwu::ref<IW::IDevice>& device)
	{
		device->DestroyVertexArray(VertexArray);
		device->DestroyIndexBuffer(IndexBuffer);
		delete this;
	}

	void Mesh::Draw(
		const iwu::ref<IW::IDevice>& device)
	{
		Material->Use(device);
		device->SetVertexArray(VertexArray);
		device->SetIndexBuffer(IndexBuffer);
		device->DrawElements(Topology, IndexCount, 0);
	}
}
