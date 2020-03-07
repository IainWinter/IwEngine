#pragma once

#include "IwGraphics.h"
#include "Bone.h"
#include "Material.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/ref.h"

namespace iw {
namespace Graphics {
	// Honestly have no clue on the best way to seperate this data...
	// Seems like the worst case is to seperate the data into different VBOS which is whats going on here
	//    , but idk how bad that actually is cus I like the idea more...

	struct Mesh {
		ref<vector3[]> Vertices;
		ref<vector3[]> Normals;
		ref<vector3[]> Tangents;
		ref<vector3[]> BiTangents;
		ref<vector4[]> Colors;
		ref<vector2[]> Uvs;

		ref<unsigned[]> Indices;

		ref<Bone[]> Bones;

		unsigned VertexCount;
		unsigned IndexCount;
		unsigned BoneCount;

		ref<Material> Material;

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
		~Mesh() = default;

		IWGRAPHICS_API
		void Initialize(
			const ref<IDevice>& device);

		IWGRAPHICS_API
		void Update(
			const ref<IDevice>& device);

		IWGRAPHICS_API
		void Destroy(
			const ref<IDevice>& device);

		IWGRAPHICS_API
		void Draw(
			const ref<IDevice>& device) const;

		IWGRAPHICS_API
		Mesh Instance() const; // makes a copy but references the same data
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
		size_t GetElementCount();
	};
}

	using namespace Graphics;
}
