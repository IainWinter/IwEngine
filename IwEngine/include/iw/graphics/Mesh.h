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

namespace detail {
	IWGRAPHICS_API
	void DestroyMeshData(
		MeshData* diekiddo);
}

	enum class bName {
		POSITION,
		NORMAL,
		TANGENT,
		BITANGENT,
		COLOR, COLOR1, COLOR2, COLOR3, COLOR4, COLOR5, COLOR6, COLOR7,
		UV,    UV1,    UV2,    UV3,    UV4,    UV5,    UV6,    UV7
	};

	struct MeshDescription {
	private:
		std::vector<VertexBufferLayout> m_layouts;
		std::unordered_map<bName, unsigned> m_map;

	public:
		IWGRAPHICS_API
		void DescribeBuffer(
			bName name,
			VertexBufferLayout& layout);

		IWGRAPHICS_API
		bool HasBuffer(
			bName name) const;

		IWGRAPHICS_API
		unsigned GetBufferCount() const;

		IWGRAPHICS_API
		unsigned GetBufferIndex(
			bName name) const;

		IWGRAPHICS_API
		VertexBufferLayout GetBufferLayout(
			bName name) const;

		IWGRAPHICS_API
		VertexBufferLayout GetBufferLayout(
			unsigned index) const;
	};

	struct MeshData {
	private:
		struct BufferData {
			ref<char[]> Data;
			unsigned Count;

			void*       Ptr()       { return Data.get(); }
			const void* Ptr() const { return Data.get(); }
		};

		struct IndexData {
			ref<unsigned[]> Index;
			unsigned Count;

			unsigned*       Ptr()       { return Index.get(); }
			const unsigned* Ptr() const { return Index.get(); }
		};

		MeshDescription m_description;
		MeshTopology    m_topology;

		IndexData               m_index;
		std::vector<BufferData> m_buffers;

		IVertexArray* m_vertexArray;
		IIndexBuffer* m_indexBuffer;

		bool m_outdated;
		bool m_bound;

		ref<MeshData> m_this;

	public:
		IWGRAPHICS_API
		MeshData();

		IWGRAPHICS_API
		MeshData(
			const MeshDescription& description);

		IWGRAPHICS_API
		Mesh MakeInstance();

		IWGRAPHICS_API
		bool IsInitialized() const;

		IWGRAPHICS_API
		bool IsOutdated() const;

		IWGRAPHICS_API
		const MeshDescription& Description() const;

		IWGRAPHICS_API
		MeshTopology Topology() const;

		IWGRAPHICS_API void*       Get     (bName name);
		IWGRAPHICS_API const void* Get     (bName name) const;
		IWGRAPHICS_API unsigned    GetCount(bName name) const;

		IWGRAPHICS_API unsigned*       GetIndex();
		IWGRAPHICS_API const unsigned* GetIndex()      const;
		IWGRAPHICS_API unsigned        GetIndexCount() const;

		IWGRAPHICS_API
		void SetTopology(
			MeshTopology topology);

		IWGRAPHICS_API
		void SetBufferData(
			bName name,
			unsigned count,
			void* data);

		IWGRAPHICS_API
		void SetIndexData(
			unsigned count,
			unsigned* data);

		IWGRAPHICS_API
		void GenNormals(
			bool smooth = true);

		IWGRAPHICS_API
		void GenTangents(
			bool smooth = true);

		IWGRAPHICS_API void Initialize(const ref<IDevice>& device);		 // Send the mesh data to video memory
		IWGRAPHICS_API void Update    (const ref<IDevice>& device);		 // Updates the video memory copy of the mesh
		IWGRAPHICS_API void Destroy   (const ref<IDevice>& device);		 // Destroys the video memory copy of the mesh
	private:
		~MeshData() = default; // force dynamic allocation

		friend void detail::DestroyMeshData(MeshData*);

		IWGRAPHICS_API       BufferData& GetBuffer(unsigned index);
		IWGRAPHICS_API const BufferData& GetBuffer(unsigned index) const;

		IWGRAPHICS_API       IndexData& GetIndexBuffer();
		IWGRAPHICS_API const IndexData& GetIndexBuffer() const;

		friend struct Mesh;

		IWGRAPHICS_API void Bind  (const ref<IDevice>& device);	  // Binds the mesh for use
		IWGRAPHICS_API void Unbind(const ref<IDevice>& device);	  // Marks mesh as unbound (doesn't change renderer state [for now])
		IWGRAPHICS_API void Draw  (const ref<IDevice>& device) const; // Draws the mesh once used
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
		Mesh MakeInstance() const;

		IWGRAPHICS_API       ref<iw::Material> Material();
		IWGRAPHICS_API const ref<iw::Material> Material() const;

		IWGRAPHICS_API       ref<iw::MeshData> Data();
		IWGRAPHICS_API const ref<iw::MeshData> Data() const;

		IWGRAPHICS_API
		void SetMaterial(
			ref<iw::Material>& material);

		IWGRAPHICS_API
		void SetData(
			ref<MeshData>& data);

		IWGRAPHICS_API void Bind  (const ref<IDevice>& device);       // Binds the mesh for use
		IWGRAPHICS_API void Unbind(const ref<IDevice>& device);       // Marks mesh as unbound (doesn't change renderer state [for now])
		IWGRAPHICS_API void Draw  (const ref<IDevice>& device) const; // Draws the mesh once used

		//IWGRAPHICS_API const ref<MeshData> Data() const;
		//IWGRAPHICS_API       ref<MeshData> Data();

		/*size_t Mesh::GetElementCount() {
			return IndexCount / Topology;
		}*/
	};
}

	using namespace Graphics;
}
