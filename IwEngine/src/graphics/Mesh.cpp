#pragma once

#include "iw/graphics/Mesh.h"

namespace IW {
	Mesh::Mesh()
		: Vertices(nullptr)
		, Normals(nullptr)
		, Tangents(nullptr)
		, BiTangents(nullptr)
		, Colors(nullptr)
		, Uvs(nullptr)
		, Indices(nullptr)
		, VertexCount(0)
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
		, Tangents(nullptr)
		, BiTangents(nullptr)
		, Colors(nullptr)
		, Uvs(nullptr)
		, Indices(nullptr)
		, VertexCount(0)
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
		Vertices = nullptr;

		if (count > 0) {
			Vertices = new iwm::vector3[count];
			memcpy(Vertices, vertices, count * sizeof(iwm::vector3));
		}

		VertexCount = count;
		Outdated = true;
	}

	void Mesh::SetNormals(
		size_t count,
		iwm::vector3* normals)
	{
		delete[] Normals;
		Normals = nullptr;

		if (count > 0) {
			Normals = new iwm::vector3[count];
			memcpy(Normals, normals, count * sizeof(iwm::vector3));
		}

		Outdated = true;
	}

	void Mesh::SetTangents(
		size_t count, 
		iwm::vector3* tangents)
	{
		delete[] Tangents;
		Tangents = nullptr;

		if (count > 0) {
			Tangents = new iwm::vector3[count];
			memcpy(Tangents, tangents, count * sizeof(iwm::vector3));
		}

		Outdated = true;
	}

	void Mesh::SetBiTangents(
		size_t count, 
		iwm::vector3* bitangents)
	{
		delete[] BiTangents;
		BiTangents = nullptr;

		if (count > 0) {
			BiTangents = new iwm::vector3[count];
			memcpy(BiTangents, bitangents, count * sizeof(iwm::vector3));
		}

		Outdated = true;
	}

	void Mesh::SetColors(
		size_t count,
		iwm::vector4* colors)
	{
		delete[] Colors;
		Colors = nullptr;

		if (count > 0) {
			Colors = new iwm::vector4[count];
			memcpy(Colors, colors, count * sizeof(iwm::vector4));
		}

		Outdated = true;
	}

	void Mesh::SetUVs(
		size_t count,
		iwm::vector2* uvs)
	{
		delete[] Uvs;
		Uvs = nullptr;

		if (count > 0) {
			Uvs = new iwm::vector2[count];
			memcpy(Uvs, uvs, count * sizeof(iwm::vector2));
		}

		Outdated = true;
	}

	void Mesh::SetIndices(
		size_t count,
		unsigned int* indices)
	{
		delete[] Indices;
		Indices = nullptr;

		if (count > 0) {
			Indices = new unsigned int[count];
			memcpy(Indices, indices, count * sizeof(unsigned int));
		}

		IndexCount = count;
		Outdated = true;
	}

	// Generate vertex normals by averaging the face normals of the surrounding faces
	void Mesh::GenNormals() {
		if (!Vertices) return;

		Normals = new iwm::vector3[VertexCount];

		for (size_t i = 0; i < IndexCount; i += 3) {
			iwm::vector3& v1 = Vertices[Indices[i + 0]];
			iwm::vector3& v2 = Vertices[Indices[i + 1]];
			iwm::vector3& v3 = Vertices[Indices[i + 2]];

			iwm::vector3 face = (v2 - v1).cross(v3 - v1).normalized();

			Normals[Indices[i + 0]] = (Normals[Indices[i + 0]] + face).normalized();
			Normals[Indices[i + 1]] = (Normals[Indices[i + 1]] + face).normalized();
			Normals[Indices[i + 2]] = (Normals[Indices[i + 2]] + face).normalized();
		}
	}

	// Generate tangents and bitangents from vertex normals and uv corrds
	// If there are no normals, they are generated along with the tangents
	void Mesh::GenTangents() {
		if (!Uvs || !Vertices) return;

		if (!Normals) {
			GenNormals();
		}

		delete[] Tangents;
		delete[] BiTangents;

		Tangents   = new iwm::vector3[VertexCount];
		BiTangents = new iwm::vector3[VertexCount];

		size_t v = 0;
		for (size_t i = 0; i < IndexCount; i += 3) {
			iwm::vector3& norm = Normals [Indices[i + 0]]; // can use any normal
			iwm::vector3& pos1 = Vertices[Indices[i + 0]];
			iwm::vector3& pos2 = Vertices[Indices[i + 1]];
			iwm::vector3& pos3 = Vertices[Indices[i + 2]];
			iwm::vector2& uv1 = Uvs[Indices[i + 0]];
			iwm::vector2& uv2 = Uvs[Indices[i + 1]];
			iwm::vector2& uv3 = Uvs[Indices[i + 2]];

			iwm::vector3 edge1 = pos2 - pos1;
			iwm::vector3 edge2 = pos3 - pos1;
			iwm::vector2 duv1  = uv2 - uv1;
			iwm::vector2 duv2  = uv3 - uv1;

			float f = 1.0f / duv1.cross_length(duv2);

			iwm::vector3 tanget;
			tanget.x = f * (duv2.y * edge1.x - duv1.y * edge2.x);
			tanget.y = f * (duv2.y * edge1.y - duv1.y * edge2.y);
			tanget.z = f * (duv2.y * edge1.z - duv1.y * edge2.z);
			tanget.normalize();

			iwm::vector3 bitanget = norm.cross(tanget);
			bitanget.normalize();

			Tangents[Indices[i + 0]] = tanget;
			Tangents[Indices[i + 1]] = tanget;
			Tangents[Indices[i + 2]] = tanget;

			BiTangents[Indices[i + 0]] = bitanget;
			BiTangents[Indices[i + 1]] = bitanget;
			BiTangents[Indices[i + 2]] = bitanget;
		}
	}

	void Mesh::Initialize(
		const iwu::ref<IW::IDevice>& device)
	{
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
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(VertexCount * sizeof(iwm::vector3), Normals);
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Tangents) {
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(VertexCount * sizeof(iwm::vector3), Tangents);
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (BiTangents) {
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(VertexCount * sizeof(iwm::vector3), BiTangents);
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Colors) {
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(VertexCount * sizeof(iwm::vector4), Colors);
				device->AddBufferToVertexArray(VertexArray, buffer, layout4f);
			}

			if (Uvs) {
				IW::IVertexBuffer* buffer = device->CreateVertexBuffer(VertexCount * sizeof(iwm::vector2), Uvs);
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
			device->UpdateVertexArrayData(VertexArray, index, Normals, VertexCount * sizeof(iwm::vector3));
			index++;
		}

		if (Colors) {
			device->UpdateVertexArrayData(VertexArray, index, Colors, VertexCount * sizeof(iwm::vector4));
			index++;
		}

		if (Uvs) {
			device->UpdateVertexArrayData(VertexArray, index, Uvs, VertexCount * sizeof(iwm::vector2));
			index++;
		}
	}

	void Mesh::Destroy(
		const iwu::ref<IW::IDevice>& device)
	{
		device->DestroyVertexArray(VertexArray);
		device->DestroyIndexBuffer(IndexBuffer);
	}

	void Mesh::Draw(
		const iwu::ref<IW::IDevice>& device) const
	{
		device->SetVertexArray(VertexArray);
		device->SetIndexBuffer(IndexBuffer);
		device->DrawElements(Topology, IndexCount, 0);
	}
}
