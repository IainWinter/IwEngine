#pragma once

#include "iw/graphics/Mesh.h"

namespace iw {
namespace Graphics {
	Mesh::Mesh()
		: Vertices(nullptr)
		, Normals(nullptr)
		, Tangents(nullptr)
		, BiTangents(nullptr)
		, Colors(nullptr)
		, Uvs(nullptr)
		, Indices(nullptr)
		, Bones(nullptr)
		, BoneCount(0)
		, VertexCount(0)
		, IndexCount(0)
		, Topology(TRIANGLES)
		, VertexArray(nullptr)
		, IndexBuffer(nullptr)
		, Outdated(false)
	{}

	Mesh::Mesh(
		MeshTopology topology)
		: Vertices(nullptr)
		, Normals(nullptr)
		, Tangents(nullptr)
		, BiTangents(nullptr)
		, Colors(nullptr)
		, Uvs(nullptr)
		, Indices(nullptr)
		, Bones(nullptr)
		, BoneCount(0)
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
		delete[] Bones;
	}

	void Mesh::Initialize(
		const iw::ref<IDevice>& device)
	{
		if (Material) {
			Material->Initialize(device);
		}

		if (VertexArray) {
			Update(device);
		}

		else {
			VertexBufferLayout layout4f;
			layout4f.Push<float>(4);

			VertexBufferLayout layout3f;
			layout3f.Push<float>(3);

			VertexBufferLayout layout2f;
			layout2f.Push<float>(2);

			VertexArray = device->CreateVertexArray();
			IndexBuffer = device->CreateIndexBuffer(Indices, IndexCount);

			if (Vertices) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Vertices, VertexCount * sizeof(iw::vector3));
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Normals) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Normals, VertexCount * sizeof(iw::vector3));
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Tangents) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Tangents, VertexCount * sizeof(iw::vector3));
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (BiTangents) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(BiTangents, VertexCount * sizeof(iw::vector3));
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Colors) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Colors, VertexCount * sizeof(iw::vector4));
				device->AddBufferToVertexArray(VertexArray, buffer, layout4f);
			}

			if (Uvs) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Uvs, VertexCount * sizeof(iw::vector2));
				device->AddBufferToVertexArray(VertexArray, buffer, layout2f);
			}
		}

		Outdated = false;
	}

	void Mesh::Update(
		const iw::ref<IDevice>& device)
	{
		if (Outdated) {
			int index = 0;
			if (Vertices) {
				device->UpdateVertexArrayData(VertexArray, index, Vertices, VertexCount * sizeof(iw::vector3));
				index++;
			}

			if (Normals) {
				device->UpdateVertexArrayData(VertexArray, index, Normals, VertexCount * sizeof(iw::vector3));
				index++;
			}

			if (Colors) {
				device->UpdateVertexArrayData(VertexArray, index, Colors, VertexCount * sizeof(iw::vector4));
				index++;
			}

			if (Uvs) {
				device->UpdateVertexArrayData(VertexArray, index, Uvs, VertexCount * sizeof(iw::vector2));
				index++;
			}

			if (IndexBuffer) {
				device->UpdateBuffer(IndexBuffer, Indices, IndexCount * sizeof(unsigned));
			}

			Outdated = false;
		}
	}

	void Mesh::Destroy(
		const iw::ref<IDevice>& device)
	{
		device->DestroyVertexArray(VertexArray);
		device->DestroyBuffer(IndexBuffer);
	}

	void Mesh::Draw(
		const iw::ref<IDevice>& device) const
	{
		device->SetVertexArray(VertexArray);
		device->SetIndexBuffer(IndexBuffer);
		device->DrawElements(Topology, IndexCount, 0);
	}

	// Generate vertex normals by averaging the face normals of the surrounding faces
	void Mesh::GenNormals() {
		if (!Vertices) return;

		Normals = new iw::vector3[VertexCount];

		for (unsigned i = 0; i < IndexCount; i += 3) {
			iw::vector3& v1 = Vertices[Indices[i + 0]];
			iw::vector3& v2 = Vertices[Indices[i + 1]];
			iw::vector3& v3 = Vertices[Indices[i + 2]];

			iw::vector3 face = (v2 - v1).cross(v3 - v1).normalized();

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

		Tangents = new iw::vector3[VertexCount];
		BiTangents = new iw::vector3[VertexCount];

		unsigned v = 0;
		for (unsigned i = 0; i < IndexCount; i += 3) {
			iw::vector3& norm = Normals[Indices[i + 0]]; // can use any normal
			iw::vector3& pos1 = Vertices[Indices[i + 0]];
			iw::vector3& pos2 = Vertices[Indices[i + 1]];
			iw::vector3& pos3 = Vertices[Indices[i + 2]];
			iw::vector2& uv1 = Uvs[Indices[i + 0]];
			iw::vector2& uv2 = Uvs[Indices[i + 1]];
			iw::vector2& uv3 = Uvs[Indices[i + 2]];

			iw::vector3 edge1 = pos2 - pos1;
			iw::vector3 edge2 = pos3 - pos1;
			iw::vector2 duv1 = uv2 - uv1;
			iw::vector2 duv2 = uv3 - uv1;

			float f = 1.0f / duv1.cross_length(duv2);

			iw::vector3 tanget;
			tanget.x = f * (duv2.y * edge1.x - duv1.y * edge2.x);
			tanget.y = f * (duv2.y * edge1.y - duv1.y * edge2.y);
			tanget.z = f * (duv2.y * edge1.z - duv1.y * edge2.z);
			tanget.normalize();

			iw::vector3 bitanget = norm.cross(tanget);
			bitanget.normalize();

			Tangents[Indices[i + 0]] = tanget;
			Tangents[Indices[i + 1]] = tanget;
			Tangents[Indices[i + 2]] = tanget;

			BiTangents[Indices[i + 0]] = bitanget;
			BiTangents[Indices[i + 1]] = bitanget;
			BiTangents[Indices[i + 2]] = bitanget;
		}
	}

	void Mesh::SetVertices(
		unsigned count,
		iw::vector3* vertices)
	{
		delete[] Vertices;
		Vertices = nullptr;

		if (count > 0) {
			Vertices = new iw::vector3[count];
			if (vertices) {
				memcpy(Vertices, vertices, count * sizeof(iw::vector3));
			}
		}

		VertexCount = count;
		Outdated = true;
	}

	void Mesh::SetNormals(
		unsigned count,
		iw::vector3* normals)
	{
		delete[] Normals;
		Normals = nullptr;

		if (count > 0) {
			Normals = new iw::vector3[count];
			if (normals) {
				memcpy(Normals, normals, count * sizeof(iw::vector3));
			}
		}

		Outdated = true;
	}

	void Mesh::SetTangents(
		unsigned count,
		iw::vector3* tangents)
	{
		delete[] Tangents;
		Tangents = nullptr;

		if (count > 0) {
			Tangents = new iw::vector3[count];
			if (tangents) {
				memcpy(Tangents, tangents, count * sizeof(iw::vector3));
			}
		}

		Outdated = true;
	}

	void Mesh::SetBiTangents(
		unsigned count,
		iw::vector3* bitangents)
	{
		delete[] BiTangents;
		BiTangents = nullptr;

		if (count > 0) {
			BiTangents = new iw::vector3[count];
			if (bitangents) {
				memcpy(BiTangents, bitangents, count * sizeof(iw::vector3));
			}
		}

		Outdated = true;
	}

	void Mesh::SetColors(
		unsigned count,
		iw::vector4* colors)
	{
		delete[] Colors;
		Colors = nullptr;

		if (count > 0) {
			Colors = new iw::vector4[count];
			if (colors) {
				memcpy(Colors, colors, count * sizeof(iw::vector4));
			}
		}

		Outdated = true;
	}

	void Mesh::SetUVs(
		unsigned count,
		iw::vector2* uvs)
	{
		delete[] Uvs;
		Uvs = nullptr;

		if (count > 0) {
			Uvs = new iw::vector2[count];
			if (uvs) {
				memcpy(Uvs, uvs, count * sizeof(iw::vector2));
			}
		}

		Outdated = true;
	}

	void Mesh::SetIndices(
		unsigned count,
		unsigned* indices)
	{
		delete[] Indices;
		Indices = nullptr;

		if (count > 0) {
			Indices = new unsigned[count];
			if (indices) {
				memcpy(Indices, indices, count * sizeof(unsigned));
			}
		}

		IndexCount = count;
		Outdated = true;
	}

	void Mesh::SetMaterial(
		iw::ref<iw::Material>& material)
	{
			Material = material;
	}

	size_t Mesh::GetElementCount() {
			return IndexCount / Topology;
	}
}
}
