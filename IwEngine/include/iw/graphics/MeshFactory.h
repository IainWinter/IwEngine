#pragma once

#include "Mesh.h"
#include "iw/util/algorithm/pair_hash.h"
#include <unordered_map>

namespace iw {
namespace Graphics {
	//Spheres

	IWGRAPHICS_API
	MeshData MakeIcosphere(
		const MeshDescription& description,
		unsigned resolution);

	IWGRAPHICS_API
	MeshData MakeUvSphere(
		const MeshDescription& description,
		unsigned latCount,
		unsigned lonCount);

	// Capsule

	MeshData MakeCapsule(
		const MeshDescription& description,
		unsigned resolution,
		float height = 2.0f,
		float radius = 0.5f);

	//Tris

	IWGRAPHICS_API
	MeshData MakeTetrahedron(
		const MeshDescription& description,
		unsigned int resolution);

	// Plane

	IWGRAPHICS_API
	MeshData MakePlane(
		const MeshDescription& description,
		unsigned xCount,
		unsigned zCount);

namespace detail {
	using IndexPair   = std::pair<unsigned, unsigned>;
	using IndexLookup = std::unordered_map<IndexPair, unsigned, pair_hash>;
	using VertList    = std::vector<vector3>;

	// Icosphere
	static const unsigned  IcoVertCount;
	static const unsigned  IcoIndexCount;
	static const vector3*  IcoVerts;
	static const unsigned* IcoIndex;

	// Tetrahedron
	static const unsigned  TriVertCount;
	static const unsigned  TriUvCount;
	static const unsigned  TriIndexCount;
	static const vector3*  TriVerts;
	static const vector2*  TriUvs;
	static const unsigned* TriIndex;

	void SubDevideVerts(
		vector3* verts,
		unsigned* index,
		unsigned& currentIndexCount,
		unsigned& currentVertCount);

	void SubDevideUvs(
		vector2* uvs,
		const unsigned* index,
		unsigned indexCount,
		unsigned& currentUvCount);

	unsigned CreateVertexForEdge(
		IndexLookup& lookup,
		vector3* verts,
		unsigned first,
		unsigned second,
		unsigned& currentVertCount);

	unsigned CreateUvsForEdge(
		IndexLookup& lookup,
		vector2* uvs,
		unsigned first,
		unsigned second,
		unsigned& currentUvCount);
}
}

	using namespace Graphics;
}
