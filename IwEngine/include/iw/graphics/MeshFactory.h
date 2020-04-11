#pragma once

#include "Mesh.h"
#include "iw/util/algorithm/pair_hash.h"
#include <unordered_map>

namespace iw {
namespace Graphics {
	//Spheres

	IWGRAPHICS_API
	Mesh* MakeIcosphere(
		unsigned resolution);

	IWGRAPHICS_API
	Mesh* MakeUvSphere(
		unsigned latCount,
		unsigned lonCount);

	// Capsule

	Mesh* MakeCapsule(
		unsigned resolution,
		float height = 2.0f,
		float radius = 0.5f);

	//Tris

	IWGRAPHICS_API
	Mesh* MakeTetrahedron(
		unsigned int resolution);

	// Plane

	IWGRAPHICS_API
	Mesh* MakePlane(
		unsigned xCount,
		unsigned zCount);

namespace detail {
	using IndexPair   = std::pair<unsigned, unsigned>;
	using IndexLookup = std::unordered_map<IndexPair, unsigned, iw::pair_hash>;
	using VertList    = std::vector<iw::vector3>;

	// Icosphere
	static const unsigned IcoVertCount;
	static const unsigned IcoIndexCount;
	static const iw::vector3* IcoVerts;
	static const unsigned*    IcoIndex;

	// Tetrahedron
	static const unsigned TriVertCount;
	static const unsigned TriUvCount;
	static const unsigned TriIndexCount;
	static const iw::vector3* TriVerts;
	static const iw::vector2* TriUvs;
	static const unsigned*    TriIndex;

	void SubDevideVerts(
		iw::vector3* verts,
		unsigned* index,
		unsigned& currentIndexCount,
		unsigned& currentVertCount);

	void SubDevideUvs(
		iw::vector2* uvs,
		const unsigned* index,
		unsigned indexCount,
		unsigned& currentUvCount);

	unsigned CreateVertexForEdge(
		IndexLookup& lookup,
		iw::vector3* verts,
		unsigned first,
		unsigned second,
		unsigned& currentVertCount);

	unsigned CreateUvsForEdge(
		IndexLookup& lookup,
		iw::vector2* uvs,
		unsigned first,
		unsigned second,
		unsigned& currentUvCount);
}
}

	using namespace Graphics;
}
