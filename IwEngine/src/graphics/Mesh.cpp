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

	void Mesh::SetVertices(
		size_t count,
		iwm::vector3* vertices)
	{
		delete[] Vertices;

		Vertices = new iwm::vector3[count];
		VertexCount = count;
		Outdated = true;

		memcpy(Vertices, vertices, count * sizeof(iwm::vector3));
	}

	void Mesh::SetNormals(
		size_t count,
		iwm::vector3* normals)
	{
		delete[] Normals;

		Normals = new iwm::vector3[count];
		NormalCount = count;
		Outdated = true;

		memcpy(Normals, normals, count * sizeof(iwm::vector3));
	}

	void Mesh::SetColors(
		size_t count,
		iwm::vector4* colors)
	{
		delete[] Colors;

		Colors = new iwm::vector4[count];
		ColorCount = count;
		Outdated = true;

		memcpy(Colors, colors, count * sizeof(iwm::vector4));
	}

	void Mesh::SetUVs(
		size_t count,
		iwm::vector2* uvs)
	{
		delete[] Uvs;

		Uvs = new iwm::vector2[count];
		UvCount = count;
		Outdated = true;

		memcpy(Uvs, uvs, count * sizeof(iwm::vector2));
	}

	void Mesh::SetIndices(
		size_t count,
		unsigned int* indices)
	{
		delete[] Indices;

		Indices = new unsigned int[count];
		IndexCount = count;
		Outdated = true;

		memcpy(Indices, indices, count * sizeof(unsigned int));
	}

	void Mesh::Compile(
		const iwu::ref<IW::IDevice>& device)
	{
		size_t min = VertexCount + NormalCount + ColorCount + UvCount;
		if (Vertices && VertexCount < min) min = VertexCount;
		if (Normals  && NormalCount < min) min = NormalCount;
		if (Colors   && ColorCount  < min) min = ColorCount;
		if (Uvs      && UvCount     < min) min = UvCount;

		if (Vertices) VertexCount = min;
		if (Normals)  NormalCount = min;
		if (Colors)   ColorCount  = min;
		if (Uvs)      UvCount     = min;

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
			IW::VertexBufferLayout layout4f;
			layout4f.Push<float>(4);

			IW::VertexBufferLayout layout3f;
			layout3f.Push<float>(3);

			IW::VertexBufferLayout layout2f;
			layout2f.Push<float>(2);

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
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(ColorCount * sizeof(iwm::vector4), Colors);
				device->AddBufferToVertexArray(VertexArray, buffer, layout4f);
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
			device->UpdateVertexArrayData(VertexArray, index, Vertices, VertexCount * sizeof(iwm::vector3));
			index++;
		}

		if (Normals) {
			device->UpdateVertexArrayData(VertexArray, index, Normals, NormalCount * sizeof(iwm::vector3));
			index++;
		}

		if (Colors) {
			device->UpdateVertexArrayData(VertexArray, index, Colors, ColorCount * sizeof(iwm::vector4));
			index++;
		}

		if (Uvs) {
			device->UpdateVertexArrayData(VertexArray, index, Uvs, UvCount * sizeof(iwm::vector2));
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
		const iwu::ref<IW::IDevice>& device) const
	{
		device->SetVertexArray(VertexArray);
		device->SetIndexBuffer(IndexBuffer);
		device->DrawElements(Topology, IndexCount, 0);
	}
}
