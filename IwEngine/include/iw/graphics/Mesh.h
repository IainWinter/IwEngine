#pragma once

#include "IwGraphics.h"
#include "Bone.h"
#include "Material.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/ref.h"

namespace IW {
namespace Graphics {
	// Honestly have no clue on the best way to seperate this data...
	// Seems like the worst case is to seperate the data into different VBOS which is whats going on here
	//    , but idk how bad that actually is cus I like the idea more...

	struct Mesh {
		iw::vector3* Vertices; // should combine
		iw::vector3* Normals;
		iw::vector3* Tangents;
		iw::vector3* BiTangents;
		iw::vector4* Colors;
		iw::vector2* Uvs;

		unsigned* Indices;

		Bone* Bones;

		unsigned VertexCount;
		unsigned IndexCount;
		unsigned BoneCount;

		iw::ref<Material> Material;

		MeshTopology Topology;

		IVertexArray* VertexArray;
		IIndexBuffer* IndexBuffer;

		bool Outdated;

		//Mesh* Next;
		//Mesh* Child;

		IWGRAPHICS_API
		Mesh();

		IWGRAPHICS_API
		Mesh(
			MeshTopology topology);

		IWGRAPHICS_API
		~Mesh();

		IWGRAPHICS_API
		void SetVertices(
			unsigned count,
			iw::vector3* vertices);

		IWGRAPHICS_API
		void SetNormals(
			unsigned count,
			iw::vector3* normals);

		IWGRAPHICS_API
		void SetTangents(
			unsigned count,
			iw::vector3* tangents);

		IWGRAPHICS_API
		void SetBiTangents(
			unsigned count,
			iw::vector3* bitangents);

		IWGRAPHICS_API
		void SetColors(
			unsigned count,
			iw::vector4* colors);

		IWGRAPHICS_API
		void SetUVs(
			unsigned count,
			iw::vector2* uvs);

		IWGRAPHICS_API
		void SetIndices(
			unsigned count,
			unsigned* indices);

		IWGRAPHICS_API
		void GenNormals();

		IWGRAPHICS_API
		void GenTangents(); 

		IWGRAPHICS_API
		void SetMaterial(
			iw::ref<IW::Material>& material);

		IWGRAPHICS_API
		size_t GetElementCount();

		IWGRAPHICS_API
		void Initialize(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		void Update(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		void Destroy(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		void Draw(
			const iw::ref<IDevice>& device) const;
	};
}

	using namespace Graphics;
}
