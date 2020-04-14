#pragma once

#include "IwGraphics.h"
#include "Material.h"
#include "Bone.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/ref.h"

#include <initializer_list>

#include <functional>

namespace iw {
namespace Graphics {
	struct MeshDescription;
	struct MeshData;
	struct Mesh;

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
			(DescribeBuffer(buffers), ...);
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
			return m_layouts[index];
		}
	};

	struct MeshData {
	private:
		struct BufferData {
			ref<char[]> Data;
			unsigned Count;

			void* Ptr() {
				return Data.get();
			}
		};

		struct IndexData {
			ref<unsigned[]> Index;
			unsigned Count;

			unsigned* Ptr() {
				return Index.get();
			}
		};

		MeshDescription m_description;
		MeshTopology    m_topology;

		std::vector<BufferData> m_buffers;

		IVertexArray* m_vertexArray;
		IIndexBuffer* m_indexBuffer;

		bool m_outdated;

	public:
		MeshData(
			const MeshDescription& description);

		Mesh GetInstance();

		BufferData GetBuffer(unsigned index);
		IndexData  GetIndexBuffer();

		MeshTopology           Topology()    const;
		const MeshDescription& Description() const;

		void SetBufferData(
			unsigned index,
			unsigned count,
			void* data);

		void SetIndexData(
			unsigned count,
			unsigned* data);

		void SetTopology(
			MeshTopology topology);

		IWGRAPHICS_API
		void GenNormals(
			bool smooth = true);

		IWGRAPHICS_API
		void GenTangents(
			bool smooth = true);

		IWGRAPHICS_API void Initialize(const ref<IDevice>& device);		 // Send the mesh data to video memory
		IWGRAPHICS_API void Update    (const ref<IDevice>& device);		 // Updates the video memory copy of the mesh
		IWGRAPHICS_API void Destroy   (const ref<IDevice>& device);		 // Destroys the video memory copy of the mesh
		IWGRAPHICS_API void Draw      (const ref<IDevice>& device) const; // Draws the mesh once used
		IWGRAPHICS_API void Bind      (const ref<IDevice>& device);		 // Binds the mesh for use
		IWGRAPHICS_API void Unbind    (const ref<IDevice>& device);		 // Marks mesh as unbound (doesn't change renderer state [for now])
	};

	struct Mesh {
	private:
		ref<MeshData> m_data;
		ref<Material> m_material;

	public:
		IWGRAPHICS_API
		Mesh() = default;

		IWGRAPHICS_API
		Mesh(
			ref<MeshData> data);

		IWGRAPHICS_API
		void SetMaterial(
			ref<Material>& material);

		IWGRAPHICS_API const ref<iw::Material> Material() const;
		IWGRAPHICS_API       ref<iw::Material> Material();

		//IWGRAPHICS_API const ref<MeshData> Data() const;
		//IWGRAPHICS_API       ref<MeshData> Data();

		/*size_t Mesh::GetElementCount() {
			return IndexCount / Topology;
		}*/
	};
}

	using namespace Graphics;
}
