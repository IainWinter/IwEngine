#pragma once

#include "Mesh.h"
#include "iw/util/algorithm/pair_hash.h"
#include <unordered_map>

namespace IW {
namespace Graphics {
	//Spheres

	IWGRAPHICS_API
	Mesh* MakeIcosphere(
		unsigned resolution);

	IWGRAPHICS_API
	Mesh* MakeUvSphere(
		unsigned latCount,
		unsigned lonCount);

	// Plane

	IWGRAPHICS_API
	Mesh* MakePlane(
		unsigned xCount,
		unsigned zCount);

namespace detail {
	using IndexPair   = std::pair<unsigned, unsigned>;
	using IndexLookup = std::unordered_map<IndexPair, unsigned, iw::pair_hash>;
	using VertList    = std::vector<iw::vector3>;

	static const unsigned IcoVertCount;
	static const unsigned IcoIndexCount;
	static const unsigned*    IcoIndex;
	static const iw::vector3* IcoVerts;

	void SubDevide(
		iw::vector3* verts,
		unsigned* index,
		unsigned& currentIndexCount,
		unsigned& currentVertCount);

	unsigned CreateVertexForEdge(
		IndexLookup& lookup,
		iw::vector3* verts,
		unsigned first,
		unsigned second,
		unsigned& currentVertCount);
}
}

	using namespace Graphics;
}
