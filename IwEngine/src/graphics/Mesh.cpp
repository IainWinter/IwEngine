#pragma once

#include "iw/graphics/Mesh.h"

namespace iw {
namespace Graphics {
	MeshData::MeshData(
		const MeshDescription& description)
		: m_description(description)
	{
		m_buffers.resize((size_t)1 + description.GetBufferCount());
	}

	Mesh MeshData::GetInstance() {
		Mesh mesh(ref<MeshData>(this));
	}

	MeshData::BufferData MeshData::GetBuffer(
		unsigned index)
	{
		return m_buffers[index];
	}

	MeshData::IndexData MeshData::GetIndexBuffer() {
		return *(IndexData*)&m_buffers.back();
	}

	MeshTopology MeshData::Topology() const { 
		return m_topology; 
	}

	const MeshDescription& MeshData::Description() const { 
		return m_description; 
	}

	void MeshData::SetBufferData(
		unsigned index,
		unsigned count,
		void* data)
	{
		if (index >= m_buffers.size()) {
			return;
		}

		unsigned size = m_description
			.GetBufferDescription(index)
			.GetStride() * count;

		BufferData& buffer = m_buffers[index];

		buffer.Count = count;
		buffer.Data = ref<char[]>(new char[(size_t)size]);

		memcpy(buffer.Data.get(), data, size);

		m_outdated = true;
	}

	void MeshData::SetIndexData(
		unsigned count,
		unsigned* data)
	{
		unsigned size = sizeof(unsigned) * count;

		BufferData& buffer = m_buffers.back();

		buffer.Count = count;
		buffer.Data = ref<char[]>(new char[(size_t)size]);

		memcpy(buffer.Data.get(), data, size);

		m_outdated = true;
	}

	void MeshData::SetTopology(
		MeshTopology topology)
	{
		m_topology = topology;
	}

	// Generate vertex normals by averaging the face normals of the surrounding faces
	void MeshData::GenNormals(
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
	void MeshData::GenTangents(
		bool smooth)
	{
		if (!Uvs || !Vertices) return;

		if (!Normals) {
			GenNormals(smooth);
		}

		Tangents = ref<vector3[]>(new vector3[VertexCount]);
		BiTangents = ref<vector3[]>(new vector3[VertexCount]);

		unsigned v = 0;
		for (unsigned i = 0; i < IndexCount; i += 3) {
			vector3& pos1 = Vertices[Indices[i + 0]];
			vector3& pos2 = Vertices[Indices[i + 1]];
			vector3& pos3 = Vertices[Indices[i + 2]];
			vector2& uv1 = Uvs[Indices[i + 0]];
			vector2& uv2 = Uvs[Indices[i + 1]];
			vector2& uv3 = Uvs[Indices[i + 2]];

			vector3 edge1 = pos2 - pos1;
			vector3 edge2 = pos3 - pos1;
			vector2 duv1 = uv2 - uv1;
			vector2 duv2 = uv3 - uv1;

			float f = 1.0f / duv1.cross_length(duv2);

			vector3 tangent = f * (edge1 * duv2.y - edge2 * duv1.y);
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

	void MeshData::Initialize(
		const ref<IDevice>& device)
	{
		if (m_vertexArray) {
			LOG_WARNING << "Mesh data was already initialized!";
			return;
		}

		m_outdated = false;

		m_vertexArray = device->CreateVertexArray();
		m_indexBuffer = device->CreateIndexBuffer(GetIndexBuffer().Ptr(), GetIndexBuffer().Count);

		for (int i = 0; i < m_buffers.size() - 1; i++) {
			BufferData& data = GetBuffer(i);
			VertexBufferLayout& layout = m_description.GetBufferDescription(i);

			IVertexBuffer* buffer = device->CreateVertexBuffer(data.Ptr(), data.Count * layout.GetStride());
			device->AddBufferToVertexArray(m_vertexArray, buffer, layout);
		}
	}

	void MeshData::Update(
		const ref<IDevice>& device)
	{
		if (!m_vertexArray) {
			LOG_WARNING << "Mesh data was not initialized!";
			Initialize(device);
		}

		if (!m_outdated) {
			return;
		}

		m_outdated = false;

		for (int i = 0; i < m_buffers.size() - 1; i++) {
			BufferData& data = GetBuffer(i);
			VertexBufferLayout& layout = m_description.GetBufferDescription(i);

			device->UpdateVertexArrayData(m_vertexArray, i, data.Ptr(), data.Count * layout.GetStride());
		}
	}

	void MeshData::Destroy(
		const ref<IDevice>& device)
	{
		if (m_vertexArray && m_indexBuffer) {
			device->DestroyVertexArray(m_vertexArray);
			device->DestroyBuffer(m_indexBuffer);
		}
	}

	void MeshData::Draw(
		const ref<IDevice>& device) const
	{
#ifdef IW_DEBUG
		if (!m_used) {
			LOG_WARNING << "Mesh needs to be used before drawing";
		}

		if (!VertexArray) {
			LOG_WARNING << "Mesh needs to be initialized!";
		}
#endif
		device->DrawElements(m_topology, GetIndexBuffer().Count, 0);
	}

	void MeshData::Bind(
		const ref<IDevice>& device)
	{
#ifdef IW_DEBUG
		m_used = true;
#endif
		device->SetVertexArray(m_vertexArray);
		device->SetIndexBuffer(m_indexBuffer);
	}

	void MeshData::Unbind(
		const ref<IDevice>& device)
	{
#ifdef IW_DEBUG
		m_used = false;
#endif
	}

	// Mesh

	Mesh::Mesh(
		ref<MeshData> data)
		: m_data(data)
	{}

	void Mesh::SetMaterial(
		ref<iw::Material>& material)
	{
		m_material = material;
	}

	const ref<iw::Material> Mesh::Material() const {
		return m_material;
	}

	ref<iw::Material> Mesh::Material() {
		return m_material;
	}
}
