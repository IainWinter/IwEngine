#pragma once

#include "IwGraphics.h"
#include "Bone.h"
#include "Material.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"

namespace IW {
inline namespace Graphics {
	// Honestly have no clue on the best way to seperate this data...
	// Seems like the worst case is to seperate the data into different VBOS which is whats going on here
	//    , but idk how bad that actually is cus I like the idea more...

	struct IWGRAPHICS_API Mesh {
		iw::vector3* Vertices; // should combine
		iw::vector3* Normals;
		iw::vector3* Tangents;
		iw::vector3* BiTangents;
		iw::vector4* Colors;
		iw::vector2* Uvs;
		unsigned int* Indices;

		size_t VertexCount;
		size_t IndexCount;

		Bone* Bones;
		size_t BoneCount;

		iw::ref<Material> Material;

		IW::MeshTopology Topology;

		IW::IVertexArray* VertexArray;
		IW::IIndexBuffer* IndexBuffer;

		bool Outdated;

		//Mesh* Next;
		//Mesh* Child;

		Mesh();

		Mesh(
			IW::MeshTopology topology);

		~Mesh();

		void SetVertices(
			size_t count,
			iw::vector3* vertices);

		void SetNormals(
			size_t count,
			iw::vector3* normals);

		void SetTangents(
			size_t count,
			iw::vector3* tangents);

		void SetBiTangents(
			size_t count,
			iw::vector3* bitangents);

		void SetColors(
			size_t count,
			iw::vector4* colors);

		void SetUVs(
			size_t count,
			iw::vector2* uvs);

		void SetIndices(
			size_t count,
			unsigned int* indices);

		void GenNormals();

		void GenTangents(); 

		inline void SetMaterial(
			iw::ref<IW::Material>& material)
		{
			Material = material;
		}

		inline size_t GetElementCount() {
			return IndexCount / Topology;
		}

		void Initialize(
			const iw::ref<IW::IDevice>& device);

		void Update(
			const iw::ref<IW::IDevice>& device);

		void Destroy(
			const iw::ref<IW::IDevice>& device);

		void Draw(
			const iw::ref<IW::IDevice>& device) const;
	};
}
}
