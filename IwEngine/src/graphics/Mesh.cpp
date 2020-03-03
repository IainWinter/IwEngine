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

	//Mesh::~Mesh() {
	//	delete[] Vertices;
	//	delete[] Normals;
	//	delete[] Colors;
	//	delete[] Uvs;
	//	delete[] Indices;
	//	delete[] Bones;
	//}

	void Mesh::Initialize(
		const ref<IDevice>& device)
	{
		Outdated = false;

		if (Material) {
			Material->Initialize(device);
		}

		if (VertexArray) {
			Update(device);
		}

		else {
			if (   !Vertices
				&& !Normals
				&& !Tangents
				&& !BiTangents
				&& !Colors
				&& !Uvs)
			{
				return; // exit if there are no buffers
			}

			VertexBufferLayout layout4f;
			layout4f.Push<float>(4);

			VertexBufferLayout layout3f;
			layout3f.Push<float>(3);

			VertexBufferLayout layout2f;
			layout2f.Push<float>(2);

			VertexArray = device->CreateVertexArray();
			IndexBuffer = device->CreateIndexBuffer(Indices.get(), IndexCount);

			if (Vertices) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Vertices.get(), VertexCount * sizeof(vector3));
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Normals) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Normals.get(), VertexCount * sizeof(vector3));
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Tangents) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Tangents.get(), VertexCount * sizeof(vector3));
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (BiTangents) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(BiTangents.get(), VertexCount * sizeof(vector3));
				device->AddBufferToVertexArray(VertexArray, buffer, layout3f);
			}

			if (Colors) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Colors.get(), VertexCount * sizeof(vector4));
				device->AddBufferToVertexArray(VertexArray, buffer, layout4f);
			}

			if (Uvs) {
				IVertexBuffer* buffer = device->CreateVertexBuffer(Uvs.get(), VertexCount * sizeof(vector2));
				device->AddBufferToVertexArray(VertexArray, buffer, layout2f);
			}
		}
	}

	void Mesh::Update(
		const ref<IDevice>& device)
	{
		if (!VertexArray) {
			Initialize(device);
		}

		if (Outdated) {
			int index = 0;
			if (Vertices) {
				device->UpdateVertexArrayData(VertexArray, index, Vertices.get(), VertexCount * sizeof(vector3));
				index++;
			}

			if (Normals) {
				device->UpdateVertexArrayData(VertexArray, index, Normals.get(), VertexCount * sizeof(vector3));
				index++;
			}

			if (Colors) {
				device->UpdateVertexArrayData(VertexArray, index, Colors.get(), VertexCount * sizeof(vector4));
				index++;
			}

			if (Uvs) {
				device->UpdateVertexArrayData(VertexArray, index, Uvs.get(), VertexCount * sizeof(vector2));
				index++;
			}

			if (IndexBuffer) {
				device->UpdateBuffer(IndexBuffer, Indices.get(), IndexCount * sizeof(unsigned));
			}

			Outdated = false;
		}
	}

	void Mesh::Destroy(
		const ref<IDevice>& device)
	{
		device->DestroyVertexArray(VertexArray);
		device->DestroyBuffer(IndexBuffer);
	}

	void Mesh::Draw(
		const ref<IDevice>& device) const
	{
		device->SetVertexArray(VertexArray);
		device->SetIndexBuffer(IndexBuffer);
		device->DrawElements(Topology, IndexCount, 0);
	}

	Mesh* Mesh::Instance() const {
		Mesh* mesh = new Mesh();
		*mesh = *this;
		return mesh;
	}

	// Generate vertex normals by averaging the face normals of the surrounding faces
	void Mesh::GenNormals(
		bool smooth)
	{
		if (!Vertices) return;

		Normals = ref<vector3[]>(new vector3[VertexCount]);

		for (unsigned i = 0; i < IndexCount; i += 3) {
			vector3& v1 = Vertices[Indices[i + 0]];
			vector3& v2 = Vertices[Indices[i + 1]];
			vector3& v3 = Vertices[Indices[i + 2]];

			vector3 face = (v2 - v1).cross(v3 - v1).normalized();

			if (smooth) {
				Normals[Indices[i + 0]] = (Normals[Indices[i + 0]] + face).normalized();
				Normals[Indices[i + 1]] = (Normals[Indices[i + 1]] + face).normalized();
				Normals[Indices[i + 2]] = (Normals[Indices[i + 2]] + face).normalized();
			}

			else {
				Normals[Indices[i + 0]] = face;
				Normals[Indices[i + 1]] = face;
				Normals[Indices[i + 2]] = face;
			}
		}
	}

	// Generate tangents and bitangents from vertex normals and uv corrds
	// If there are no normals, they are generated along with the tangents
	void Mesh::GenTangents(
		bool smooth)
	{
		if (!Uvs || !Vertices) return;

		if (!Normals) {
			GenNormals(smooth);
		}

		Tangents   = ref<vector3[]>(new vector3[VertexCount]);
		BiTangents = ref<vector3[]>(new vector3[VertexCount]);

		unsigned v = 0;
		for (unsigned i = 0; i < IndexCount; i += 3) {
			vector3& pos1 = Vertices[Indices[i + 0]];
			vector3& pos2 = Vertices[Indices[i + 1]];
			vector3& pos3 = Vertices[Indices[i + 2]];
			vector2& uv1  = Uvs     [Indices[i + 0]];
			vector2& uv2  = Uvs     [Indices[i + 1]];
			vector2& uv3  = Uvs     [Indices[i + 2]];

			vector3 edge1 = pos2 - pos1;
			vector3 edge2 = pos3 - pos1;
			vector2 duv1  = uv2  - uv1;
			vector2 duv2  = uv3  - uv1;

			float f = 1.0f / duv1.cross_length(duv2);

			vector3 tangent   = f * (edge1 * duv2.y - edge2 * duv1.y);
			vector3 bitangent = f * (edge2 * duv1.x - edge1 * duv2.x);
			
			tangent.normalize();
			bitangent.normalize();

			Tangents[Indices[i + 0]] = tangent;
			Tangents[Indices[i + 1]] = tangent;
			Tangents[Indices[i + 2]] = tangent;

			BiTangents[Indices[i + 0]] = bitangent;
			BiTangents[Indices[i + 1]] = bitangent;
			BiTangents[Indices[i + 2]] = bitangent;
		}
	}

	void Mesh::SetVertices(
		unsigned count,
		vector3* vertices)
	{
		if (count > 0) {
			Vertices = ref<vector3[]>(new vector3[count]);
			if (vertices) {
				memcpy(Vertices.get(), vertices, count * sizeof(vector3));
			}
		}

		VertexCount = count;
		Outdated = true;
	}

	void Mesh::SetNormals(
		unsigned count,
		vector3* normals)
	{
		if (count > 0) {
			Normals = ref<vector3[]>(new vector3[count]);
			if (normals) {
				memcpy(Normals.get(), normals, count * sizeof(vector3));
			}
		}

		Outdated = true;
	}

	void Mesh::SetTangents(
		unsigned count,
		vector3* tangents)
	{
		if (count > 0) {
			Tangents = ref<vector3[]>(new vector3[count]);
			if (tangents) {
				memcpy(Tangents.get(), tangents, count * sizeof(vector3));
			}
		}

		Outdated = true;
	}

	void Mesh::SetBiTangents(
		unsigned count,
		vector3* bitangents)
	{
		if (count > 0) {
			BiTangents = ref<vector3[]>(new vector3[count]);
			if (bitangents) {
				memcpy(BiTangents.get(), bitangents, count * sizeof(vector3));
			}
		}

		Outdated = true;
	}

	void Mesh::SetColors(
		unsigned count,
		vector4* colors)
	{
		if (count > 0) {
			Colors = ref<vector4[]>(new vector4[count]);
			if (colors) {
				memcpy(Colors.get(), colors, count * sizeof(vector4));
			}
		}

		Outdated = true;
	}

	void Mesh::SetUVs(
		unsigned count,
		vector2* uvs)
	{
		if (count > 0) {
			Uvs = ref<vector2[]>(new vector2[count]);
			if (uvs) {
				memcpy(Uvs.get(), uvs, count * sizeof(vector2));
			}
		}

		Outdated = true;
	}

	void Mesh::SetIndices(
		unsigned count,
		unsigned* indices)
	{
		if (count > 0) {
			Indices = ref<unsigned[]>(new unsigned[count]);
			if (indices) {
				memcpy(Indices.get(), indices, count * sizeof(unsigned));
			}
		}

		IndexCount = count;
		Outdated = true;
	}

	void Mesh::SetMaterial(
		ref<iw::Material>& material)
	{
		Material = material;
	}

	size_t Mesh::GetElementCount() {
		return IndexCount / Topology;
	}
}
}
