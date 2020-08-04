#pragma once

#include "iw/graphics/Mesh.h"
#include <algorithm>

namespace iw {
namespace Graphics {
namespace detail {
	void DestroyMeshData(
		MeshData* diekiddo)
	{
		delete diekiddo;
	}
}

	// Mesh Description

	MeshDescription::MeshDescription()
		: m_hasInstancedBuffer(false)
	{}

	void MeshDescription::DescribeBuffer(
		bName name,
		VertexBufferLayout& layout)
	{
		m_map.emplace(name, m_layouts.size());
		m_layouts.push_back(layout);

		if (layout.GetInstanceStride() > 0) {
			m_hasInstancedBuffer = true;
		}
	}

	void MeshDescription::RemoveBuffer(
		bName name)
	{
		if (!HasBuffer(name)) {
			LOG_WARNING << "Tried to remove buffer that does not exist in description! " << (int)name;
			return;
		}

		unsigned index = m_map.at(name);
		m_map.erase(name);

		m_layouts.erase(m_layouts.begin() + index);

		for (auto& kv : m_map) {
			if (kv.second > index) {
				kv.second--;
			}
		}
	}

	bool MeshDescription::HasBuffer(
		bName name) const
	{
		return m_map.find(name) != m_map.end();
	}

	unsigned MeshDescription::GetBufferCount() const {
		return m_layouts.size();
	}

	unsigned MeshDescription::GetBufferIndex(
		bName name) const
	{
		return m_map.at(name);
	}

	std::vector<bName> MeshDescription::GetBufferNames() const {
		std::vector<bName> names;
		names.reserve(m_map.size());

		for (auto pair : m_map) {
			names.push_back(pair.first);
		}

		return names;
	}

	VertexBufferLayout MeshDescription::GetBufferLayout(
		bName name) const
	{
		return GetBufferLayout(GetBufferIndex(name));
	}

	VertexBufferLayout MeshDescription::GetBufferLayout(
		unsigned index) const
	{
		return m_layouts.at(index);
	}

	bool MeshDescription::HasInstancedBuffer() const {
		return m_hasInstancedBuffer;
	}

	// Mesh Data

	MeshData::MeshData()
		: m_topology(MeshTopology::TRIANGLES)
		, m_vertexArray(nullptr)
		, m_indexBuffer(nullptr)
		, m_outdated(false)
		, m_bound(false)
		, m_name("unnamed")
	{
		m_this = ref<MeshData>(this, detail::DestroyMeshData);
	}

	MeshData::MeshData(
		const MeshDescription& description)
		: m_description(description)
		, m_topology(MeshTopology::TRIANGLES)
		, m_vertexArray(nullptr)
		, m_indexBuffer(nullptr)
		, m_outdated(false)
		, m_bound(false)
		, m_name("unnamed")
	{
		m_this = ref<MeshData>(this, detail::DestroyMeshData);

		m_buffers.resize(description.GetBufferCount());
	}

	Mesh MeshData::MakeInstance() {
		return Mesh(m_this);
	}

	ref<MeshData> MeshData::MakeLink() {
		return MakeCopy(m_description, true);
	}

	ref<MeshData> MeshData::MakeCopy(
		const MeshDescription& links,
		bool linkIndex)
	{
		MeshData* data = new MeshData(m_description);
		data->m_topology = m_topology;
		data->m_name = m_name;

		for (bName name : m_description.GetBufferNames()) {
			unsigned index = m_description.GetBufferIndex(name); // both have same desc -> same index

			BufferData& buffer = data->GetBuffer(index);
			BufferData& other  =       GetBuffer(index);

			if (links.HasBuffer(name)) {
				buffer = other;
			}

			else {
				data->SetBufferData(name, other.Count, other.Ptr());
			}
		}

		if (linkIndex) {
			data->m_index = m_index;
		}

		else {
			data->SetIndexData(m_index.Count, m_index.Ptr());
		}

		return data->m_this;
	}

	ref<MeshData> MeshData::Subtract(
		const ref<MeshData> other) const
	{	
		MeshData* data = new MeshData();
		data->m_topology = m_topology;
		data->m_name = m_name + " subtracted from " + other->Name();

		iw::vector3* iverts = (iw::vector3*)       Get(bName::POSITION);
		iw::vector3* jverts = (iw::vector3*)other->Get(bName::POSITION);

		unsigned icount =        GetCount(bName::POSITION);
		unsigned jcount = other->GetCount(bName::POSITION);

		iw::vector3* verts = new iw::vector3[icount * jcount];
		unsigned*    index = new unsigned   [icount * jcount];

		for (unsigned i = 0; i < icount; i++) {
			for (unsigned j = 0; j < jcount; j++) {
				verts[j + i * jcount] = iverts[i] + jverts[j];
				index[j + i * jcount] = j + i * icount; // not sure
			}
		}

		//for (unsigned i = 0; i < icount * jcount; i++) {
		//	iw::vector3 a = verts[i];
		//	iw::vector3 b;
		//	float max = 0.0f;

		//	for (unsigned j = 0; j < icount * jcount; j++) {
		//		iw::vector3 v = verts[j + i * icount];
		//		float distance = (a - v).length_squared();
		//		if (distance > max) {
		//			max = distance;
		//			b = v;
		//		}
		//	}



		//}

		data->SetBufferDataPtr(bName::POSITION, icount * jcount, verts);
		data->SetIndexData    (icount * jcount, index);

		data->GenNormals();

		return data->m_this;
	}

	bool MeshData::IsInitialized() const {
		return !!m_vertexArray || !!m_indexBuffer;
	}

	bool MeshData::IsOutdated() const {
		return m_outdated;
	}

	MeshTopology MeshData::Topology() const {
		return m_topology;
	}

	const MeshDescription& MeshData::Description() const {
		return m_description;
	}

	const std::string& MeshData::Name() const {
		return m_name;
	}

	void* MeshData::Get(
		bName name)
	{
		return GetBuffer(m_description.GetBufferIndex(name)).Ptr();
	}

	const void* MeshData::Get(
		bName name) const
	{
		return GetBuffer(m_description.GetBufferIndex(name)).Ptr();
	}

	unsigned MeshData::GetCount(
		bName name) const
	{
		return GetBuffer(m_description.GetBufferIndex(name)).Count;
	}

	unsigned* MeshData::GetIndex() {
		return GetIndexBuffer().Ptr();
	}

	const unsigned* MeshData::GetIndex() const {
		return GetIndexBuffer().Ptr();
	}

	unsigned MeshData::GetIndexCount() const {
		return GetIndexBuffer().Count;
	}

	void MeshData::SetBufferData(
		bName name,
		unsigned count,
		void* data)
	{

		if (!m_description.HasBuffer(name)) {
			return;
		}

		std::unique_lock<std::mutex> lock(m_mutex);

		unsigned index = m_description.GetBufferIndex(name);
		unsigned size  = count * m_description.GetBufferLayout(index).GetStride();

		BufferData& buffer = m_buffers[index];

		buffer.Count = count;
		buffer.Data = ref<char[]>(new char[size]);
		buffer.m_Ptr = nullptr;

		memcpy(buffer.Ptr(), data, size);

		m_outdated = true;
	}

	void MeshData::SetBufferDataPtr(
		bName name,
		unsigned count,
		void* ptr)
	{
		if (!m_description.HasBuffer(name)) {
			return;
		}

		std::unique_lock<std::mutex> lock(m_mutex);

		unsigned index = m_description.GetBufferIndex(name);

		BufferData& buffer = m_buffers[index];

		buffer.Count = count;
		buffer.m_Ptr = ptr;

		m_outdated = true;
	}

	void MeshData::SetIndexData(
		unsigned count,
		unsigned* data)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_index.Count = count;
		m_index.Index = ref<unsigned[]>(new unsigned[count]);

		memcpy(m_index.Ptr(), data, count * sizeof(unsigned));

		m_outdated = true;
	}

	void MeshData::SetTopology(
		MeshTopology topology)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_topology = topology;
	}

	void MeshData::SetName(
		const std::string& name)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_name = name;
	}

	// Generate vertex normals by averaging the face normals of the surrounding faces
	void MeshData::GenNormals(
		bool smooth)
	{
		if (!m_description.HasBuffer(bName::POSITION)) return;

		// check if mesh description has normals

		vector3*  positions = (vector3*)Get(bName::POSITION);
		unsigned* index     = GetIndex();
		
		vector3* normals = new vector3[GetCount(bName::POSITION)];

		for (unsigned i = 0; i < GetIndexCount(); i += 3) {
			vector3& v1 = positions[index[i + 0]];
			vector3& v2 = positions[index[i + 1]];
			vector3& v3 = positions[index[i + 2]];

			vector3 face = (v2 - v1).cross(v3 - v1).normalized();

			if (smooth) {
				normals[index[i + 0]] = (normals[index[i + 0]] + face).normalized();
				normals[index[i + 1]] = (normals[index[i + 1]] + face).normalized();
				normals[index[i + 2]] = (normals[index[i + 2]] + face).normalized();
			}

			else {
				normals[index[i + 0]] = face;
				normals[index[i + 1]] = face;
				normals[index[i + 2]] = face;
			}
		}

		SetBufferData(bName::NORMAL, GetCount(bName::POSITION), normals);
	}

	// Generate tangents and bitangents from vertex normals and uv corrds
	// If there are no normals, they are generated along with the tangents
	void MeshData::GenTangents(
		bool smooth)
	{
		if (   !m_description.HasBuffer(bName::UV)
			|| !m_description.HasBuffer(bName::POSITION)
			|| GetCount(bName::UV) == 0
			|| GetCount(bName::POSITION) == 0)
		{
			return;
		}

		if (   m_description.HasBuffer(bName::NORMAL)
			&& GetCount(bName::NORMAL) == 0)
		{
			GenNormals(smooth);
		}

		vector3*  positions = (vector3*)Get(bName::POSITION);
		vector2*  uvs       = (vector2*)Get(bName::UV);
		unsigned* index     = GetIndex();

		// check if mesh description has tangents and bi tangents

		vector3* tangents  = new vector3[GetCount(bName::NORMAL)];
		vector3* btangents = new vector3[GetCount(bName::NORMAL)];

		unsigned v = 0;
		for (unsigned i = 0; i < GetIndexCount(); i += 3) {
			vector3& pos1 = positions[index[i + 0]];
			vector3& pos2 = positions[index[i + 1]];
			vector3& pos3 = positions[index[i + 2]];
			vector2& uv1 = uvs[index[i + 0]];
			vector2& uv2 = uvs[index[i + 1]];
			vector2& uv3 = uvs[index[i + 2]];

			vector3 edge1 = pos2 - pos1;
			vector3 edge2 = pos3 - pos1;
			vector2 duv1 = uv2 - uv1;
			vector2 duv2 = uv3 - uv1;

			float f = 1.0f / duv1.cross_length(duv2);

			vector3 tangent   = f * (edge1 * duv2.y - edge2 * duv1.y);
			vector3 bitangent = f * (edge2 * duv1.x - edge1 * duv2.x);

			tangent  .normalize();
			bitangent.normalize();

			tangents[index[i + 0]] = tangent;
			tangents[index[i + 1]] = tangent;
			tangents[index[i + 2]] = tangent;

			btangents[index[i + 0]] = bitangent;
			btangents[index[i + 1]] = bitangent;
			btangents[index[i + 2]] = bitangent;
		}

		SetBufferData(bName::TANGENT,   GetCount(bName::NORMAL),  tangents);
		SetBufferData(bName::BITANGENT, GetCount(bName::NORMAL), btangents);
	}

	void MeshData::ConformMeshData(
		const MeshDescription& description)
	{
		if (IsInitialized()) {
			LOG_WARNING << "Cannot conform mesh data after it has been initialized!";
			return;
		}

		std::unique_lock<std::mutex> lock(m_mutex);

		std::vector<unsigned> diff;

		for (bName name : m_description.GetBufferNames()) {
			if (!description.HasBuffer(name)) {
				diff.push_back(m_description.GetBufferIndex(name));
			}
		}

		std::sort(diff.begin(), diff.end(), std::greater<unsigned>());

		for (unsigned i : diff) {
			m_buffers.erase(m_buffers.begin() + i);
		}

		// This has only been tested with buffers in the same order 

		for (bName name : description.GetBufferNames()) {
			if (!m_description.HasBuffer(name)) {
				unsigned index = description.GetBufferIndex(name);
				m_buffers.emplace(m_buffers.begin() + index); // no args
			}
		}

		//m_buffers.resize(description.GetBufferCount());

		m_description = description;
	}

	void MeshData::TransformMeshData(
		const Transform& transform)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		if (m_description.HasBuffer(bName::POSITION)) {
			BufferData buffer = GetBuffer(m_description.GetBufferIndex(bName::POSITION));
			vector3* data     = buffer.Ptr<vector3>();

			for (int i = 0; i < buffer.Count; i++) {
				vector4 v = vector4(data[i], 1) * transform.WorldTransformation();
				data[i] = v;
			}

			m_outdated = true;
		}

		if (m_description.HasBuffer(bName::NORMAL)) {
			BufferData buffer = GetBuffer(m_description.GetBufferIndex(bName::NORMAL));
			vector3*   data   = buffer.Ptr<vector3>();

			for (int i = 0; i < buffer.Count; i++) {
				vector4 v = vector4(data[i], 1) * transform.WorldTransformation();
				data[i] = v;
			}

			m_outdated = true;
		}

		if (m_description.HasBuffer(bName::TANGENT)) {
			BufferData buffer = GetBuffer(m_description.GetBufferIndex(bName::TANGENT));
			vector3* data = buffer.Ptr<vector3>();

			for (int i = 0; i < buffer.Count; i++) {
				vector4 v = vector4(data[i], 1) * transform.WorldTransformation();
				data[i] = v;
			}

			m_outdated = true;
		}

		if (m_description.HasBuffer(bName::BITANGENT)) {
			BufferData buffer = GetBuffer(m_description.GetBufferIndex(bName::BITANGENT));
			vector3* data = buffer.Ptr<vector3>();

			for (int i = 0; i < buffer.Count; i++) {
				vector4 v = vector4(data[i], 1) * transform.WorldTransformation();
				data[i] = v;
			}

			m_outdated = true;
		}

	}

	void MeshData::Initialize(
		const ref<IDevice>& device)
	{
		if (   m_vertexArray 
			|| m_indexBuffer)
		{
			LOG_WARNING << "Mesh data was already initialized!";
			return;
		}

		std::unique_lock<std::mutex> lock(m_mutex);

		m_vertexArray = device->CreateVertexArray();
		m_indexBuffer = device->CreateIndexBuffer(GetIndexBuffer().Ptr(), GetIndexBuffer().Count);

		for (int i = 0; i < m_buffers.size(); i++) {
			BufferData& data = GetBuffer(i);
			VertexBufferLayout& layout = m_description.GetBufferLayout(i);
			TryAddBufferToVertexArray(device, layout, data, i);
		}

		m_outdated = false;
	}

	void MeshData::Update(
		const ref<IDevice>& device)
	{
		if (!m_vertexArray) {
			LOG_WARNING << "Mesh data was not initialized!";
			return;
		}

		if (!m_outdated) {
			LOG_WARNING << "Mesh data is not out of date!";
			return;
		}

		std::unique_lock<std::mutex> lock(m_mutex);

		for (int i = 0; i < m_buffers.size(); i++) {
			BufferData& data = GetBuffer(i);
			VertexBufferLayout& layout = m_description.GetBufferLayout(i);

			if (data.Initialized) {
				device->UpdateVertexArrayData(m_vertexArray, i, data.Ptr(), data.Count * layout.GetStride()); // todo: crash here
			}

			else {
				TryAddBufferToVertexArray(device, layout, data, i);
			}
		}

		IndexData& index = GetIndexBuffer();
		if (index.Ptr()) {
			device->UpdateBuffer(m_indexBuffer, index.Ptr(), index.Count * sizeof(unsigned));
		}

		m_outdated = false;
	}

	void MeshData::Destroy(
		const ref<IDevice>& device)
	{
		if (m_vertexArray && m_indexBuffer) {
			device->DestroyVertexArray(m_vertexArray);
			device->DestroyBuffer(m_indexBuffer);
		}
	}

	void MeshData::Bind(
		const ref<IDevice>& device)
	{
		m_bound = true;
		device->SetVertexArray(m_vertexArray);
		device->SetIndexBuffer(m_indexBuffer);
	}

	void MeshData::Unbind(
		const ref<IDevice>& device)
	{
		m_bound = false;
	}

	void MeshData::Draw(
		const ref<IDevice>& device) const
	{
#ifdef IW_DEBUG
		if (!m_vertexArray) {
			LOG_WARNING << "Mesh needs to be initialized before drawing!";
		}

		if (!m_bound) {
			LOG_WARNING << "Mesh needs to be bound before drawing!";
		}

		if (m_outdated) {
			LOG_WARNING << "Mesh is begin drawn out of date!";
		}
#endif

		if (m_description.HasInstancedBuffer()) {
			device->DrawElementsInstanced(m_topology, GetIndexBuffer().Count, 0, GetCount(bName::UV1));
		}

		else {
			device->DrawElements(m_topology, GetIndexBuffer().Count, 0);
		}
	}

	void MeshData::TryAddBufferToVertexArray(
		const ref<IDevice>& device,
		VertexBufferLayout& layout,
		BufferData& data,
		unsigned index)
	{
		if (!data.Ptr()) {
			return;
		}

		IVertexBuffer* buffer = device->CreateVertexBuffer(data.Ptr(), data.Count * layout.GetStride());
		device->AddBufferToVertexArray(m_vertexArray, buffer, layout, index);
		data.Initialized = true;
	}

	MeshData::BufferData& MeshData::GetBuffer(
		unsigned index)
	{
		return m_buffers[index];
	}

	const MeshData::BufferData& MeshData::GetBuffer(
		unsigned index) const
	{
		return m_buffers[index];
	}

	MeshData::IndexData& MeshData::GetIndexBuffer() {
		return m_index;
	}

	const MeshData::IndexData& MeshData::GetIndexBuffer() const {
		return m_index;
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

	Mesh Mesh::MakeInstance() const {
		Mesh mesh = *this;
		if (Material()) {
			mesh.SetMaterial(Material()->MakeInstance());
		}

		return mesh;
	}

	ref<iw::Material> Mesh::Material() {
		return m_material;
	}

	const ref<iw::Material> Mesh::Material() const {
		return m_material;
	}

	ref<iw::MeshData> Mesh::Data() {
		return m_data;
	}

	const ref<iw::MeshData> Mesh::Data() const {
		return m_data;
	}

	void Mesh::SetData(
		ref<MeshData>& data)
	{
		m_data = data;
	}

	void Mesh::Bind(
		const ref<IDevice>& device)
	{
		m_data->Bind(device);
	}

	void Mesh::Unbind(
		const ref<IDevice>& device)
	{
		m_data->Unbind(device);
	}

	void Mesh::Draw(
		const ref<IDevice>& device) const
	{
		m_data->Draw(device);
	}
}
}
