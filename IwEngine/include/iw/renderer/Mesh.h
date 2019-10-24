#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Device.h" // Temp
#include "iw/util/memory/smart_pointers.h"

namespace IwRenderer {

	// Honestly have no clue on the best way to seperate this data...
	// Seems like the worst case is to seperate the data into different VBOS which is whats going on here
	//    , but idk how bad that actually is cus I like the idea more...

	struct IWRENDERER_API Mesh {
		iwm::vector3* Vertices;
		iwm::vector3* Normals;
		iwm::vector3* Colors;
		iwm::vector2* Uvs;
		unsigned int* Indices;

		size_t VertexCount;
		size_t NormalCount;
		size_t ColorCount;
		size_t UvCount;
		size_t IndexCount;

		MeshTopology Topology;

		IVertexArray* VertexArray;
		IIndexBuffer* IndexBuffer;

		bool Outdated;

		Mesh();

		Mesh(
			MeshTopology topology);

		~Mesh();

		void Compile (
			const iwu::ref<IwRenderer::IDevice>& device);

		void Update(
			const iwu::ref<IwRenderer::IDevice>& device);

		void Destroy(
			const iwu::ref<IwRenderer::IDevice>& device);

		inline size_t GetElementCount() {
			return IndexCount / Topology;
		}

		inline void SetVertices(
			size_t count,
			iwm::vector3* vertices)
		{
			Vertices    = vertices;
			VertexCount = count;
			Outdated = true;
		}

		inline void SetNormals(
			size_t count,
			iwm::vector3* normals)
		{
			Normals     = normals;
			NormalCount = count;
			Outdated = true;
		}

		inline void SetColors(
			size_t count,
			iwm::vector3* colors)
		{
			Colors      = colors;
			ColorCount  = count;
			Outdated = true;
		}

		inline void SetUVs(
			size_t count,
			iwm::vector2* uvs)
		{
			Uvs      = uvs;
			UvCount  = count;
			Outdated = true;
		}

		inline void SetIndices(
			size_t count,
			unsigned int* indices)
		{
			Indices    = indices;
			IndexCount = count;
			Outdated = true;
		}
	};
}
