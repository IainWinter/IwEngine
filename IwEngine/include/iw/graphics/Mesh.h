#pragma once

#include "IwGraphics.h"
#include "Material.h"
#include "Bone.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/ref.h"

#include <initializer_list>

namespace iw {
namespace Graphics {
	template<
		typename _t,
		unsigned _s>
	struct vdata {};

	struct MeshDescription {
	private:
		std::vector<VertexBufferLayout> m_layouts;

	public:
		template<
			typename... _t,
			unsigned... _s>
		void DescribeMesh(
			vdata<_t, _s>... buffers)
		{
			(DescribeBuf(buffers), ...);
		}

		template<
			typename... _t,
			unsigned... _s>
		void DescribeBuffer(
			vdata<_t, _s>... verts)
		{
			VertexBufferLayout layout;
			(layout.Push<_t>(_s), ...);

			m_layouts.emplace_back(layout);
		}

		template<
			typename... _t,
			unsigned... _s>
		void DescribeInstanceBuffer(
			unsigned stride,
			vdata<_t, _s>... verts)
		{
			VertexBufferLayout layout(stride);
			(layout.Push<_t>(_s), ...);

			m_layouts.emplace_back(layout);
		}

		unsigned GetBufferCount() const {
			return m_layouts.size();
		}

		VertexBufferLayout GetBufferDescription(
			unsigned index) const
		{
			if (index >= m_layouts.size()) {
				return;
			}

			return m_layouts[index];
		}
	};

	struct MeshData {
	private:
		struct BufferData {
			ref<void> Data;
			unsigned Count;
		};

		std::vector<BufferData> m_buffers;

		MeshDescription m_description;

	public:
		MeshData(
			const MeshDescription& description)
			: m_description(description)
		{
			m_buffers.resize(description.GetBufferCount());
		}

		void SetBufferData(
			unsigned index,
			unsigned count,
			void* data)
		{
			if (index >= m_buffers.size()) {
				return;
			}

			m_description.Get

			m_buffers[index].Data = ref<void>();
		}
	};


	// Honestly have no clue on the best way to seperate this data...
	// Seems like the worst case is to seperate the data into different VBOS which is whats going on here
	//    , but idk how bad that actually is cus I like the idea more...

	struct Mesh {
		// these should all be private

		ref<vector3[]> Vertices;
		ref<vector3[]> Normals;
		ref<vector3[]> Tangents;
		ref<vector3[]> BiTangents;
		ref<vector4[]> Colors;
		ref<vector2[]> Uvs;

		ref<Bone> Bones;

		ref<unsigned[]> Indices;

		unsigned VertexCount;
		unsigned IndexCount;
		unsigned BoneCount;

		ref<Material> Material;

		MeshTopology Topology;

		IVertexArray* VertexArray;
		IIndexBuffer* IndexBuffer;

		bool Outdated;
		bool IsStatic;

		//std::vector<Mesh> m_children; // make all this private

	private:
#ifdef IW_DEBUG
		bool m_used;
#endif
	public:
		//Mesh* Next;
		//Mesh* Child;

		IWGRAPHICS_API
		Mesh();

		IWGRAPHICS_API
		Mesh(
			MeshTopology topology);

		GEN_default5(IWGRAPHICS_API, Mesh)

		// Sends the mesh to video memory
		IWGRAPHICS_API
		void Initialize(
			const ref<IDevice>& device);

		// Updates the video memory copy of the mesh
		IWGRAPHICS_API
		void Update(
			const ref<IDevice>& device);

		// Destroys the video memory copy of the mesh
		IWGRAPHICS_API
		void Destroy(
			const ref<IDevice>& device);

		// Draws the mesh once used
		IWGRAPHICS_API
		void Draw(
			const ref<IDevice>& device) const;

		// Binds the mesh for use
		IWGRAPHICS_API
		void Bind(
			const ref<IDevice>& device);

		// Marks mesh as unbound (doesn't change renderer state [for now])
		IWGRAPHICS_API
		void Unbind(
			const ref<IDevice>& device);

		IWGRAPHICS_API
		Mesh* Instance() const; // makes a copy but references the same data
		//Mesh Clone() const; // will clone the meshs data

		IWGRAPHICS_API
		void GenNormals(
			bool smooth = true);

		IWGRAPHICS_API
		void GenTangents(
			bool smooth = true);

		IWGRAPHICS_API
		void SetVertices(
			unsigned count,
			vector3* vertices);

		IWGRAPHICS_API
		void SetNormals(
			unsigned count,
			vector3* normals);

		IWGRAPHICS_API
		void SetTangents(
			unsigned count,
			vector3* tangents);

		IWGRAPHICS_API
		void SetBiTangents(
			unsigned count,
			vector3* bitangents);

		IWGRAPHICS_API
		void SetColors(
			unsigned count,
			vector4* colors);

		IWGRAPHICS_API
		void SetUVs(
			unsigned count,
			vector2* uvs);

		IWGRAPHICS_API
		void SetIndices(
			unsigned count,
			unsigned* indices);

		IWGRAPHICS_API
		void SetMaterial(
			ref<iw::Material>& material);

		IWGRAPHICS_API
		void SetIsStatic(
			bool isStatic);

		IWGRAPHICS_API
		size_t GetElementCount();
	};
}

	using namespace Graphics;
}
