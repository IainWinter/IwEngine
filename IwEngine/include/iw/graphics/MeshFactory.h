#pragma once

#include "Mesh.h"
#include "iw/util/algorithm/pair_hash.h"
#include <unordered_map>
#include <functional>

namespace iw {
namespace Graphics {
	//Spheres

	IWGRAPHICS_API
	MeshData* MakeIcosphere(
		const MeshDescription& description,
		unsigned resolution = 0);

	IWGRAPHICS_API
	MeshData* MakeUvSphere(
		const MeshDescription& description,
		unsigned latCount = 16,
		unsigned lonCount = 32);

	// Capsule

	IWGRAPHICS_API
	MeshData* MakeCapsule(
		const MeshDescription& description,
		unsigned resolution,
		float height = 2.0f,
		float radius = 0.5f);

	//Tris

	IWGRAPHICS_API
	MeshData* MakeTetrahedron(
		const MeshDescription& description,
		unsigned int resolution = 0);

	// Plane

	IWGRAPHICS_API
	MeshData* MakePlane(
		const MeshDescription& description,
		unsigned xCount = 5,
		unsigned zCount = 5);

	// Cube

	IWGRAPHICS_API
	MeshData* MakeCube(
		const MeshDescription& description, // can only return a default cube
		unsigned resolution = 0); // resolution doesnt do anything rn

	// Rock

	IWGRAPHICS_API
	MeshData* GenerateFromVoxels(
		const MeshDescription& description,
		vector3 min,
		vector3 max,
		vector3 d,
		std::vector<
			std::function<float(vector3, size_t, size_t, size_t, float***)>> passes);


namespace detail {
	using IndexPair   = std::pair<unsigned, unsigned>;
	using IndexLookup = std::unordered_map<IndexPair, unsigned, pair_hash>;
	using VertList    = std::vector<vector3>;

	// Icosphere
	static const unsigned  IcoVertCount;
	static const unsigned  IcoIndexCount;
	static const vector3*  IcoVerts;
	static const vector2*  IcoUvs;
	static const unsigned* IcoIndex;

	// Tetrahedron
	static const unsigned  TriVertCount;
	static const unsigned  TriUvCount;
	static const unsigned  TriIndexCount;
	static const vector3*  TriVerts;
	static const vector2*  TriUvs;
	static const unsigned* TriIndex;

	// Cube

	static const unsigned CubeVertCount;
	static const unsigned CubeIndexCount;
	static vector3*  CubeVerts;
	static vector2*  CubeUvs;
	static unsigned* CubeIndex;

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

	unsigned CreateVertexForEdgeVector(
		IndexLookup& lookup,
		std::vector<vector3>& verts,
		vector3* source,
		unsigned first,
		unsigned second,
		unsigned offset);

	// put this is linear_alloc, find an algo for n dimensions would be cool

	//https://stackoverflow.com/questions/340943/c-multi-dimensional-arrays-on-the-heap
	template<
		typename _t>
	_t*** new3D(
		unsigned N1,
		unsigned N2,
		unsigned N3)
	{
	    _t*** array = new _t**[N1];

	    array[0]    = new _t*[N1*N2];
	    array[0][0] = new _t [N1*N2*N3];

	    for (unsigned i = 0; i < N1; i++) {
		   if (i < N1 - 1) {
			  array[0  ][(i+1)*N2] = &(array[0][0][(i+1)*N2*N3]);
			  array[i+1]           = &(array[0   ][(i+1)*N2]);
		   }

		   for (unsigned j = 0; j < N2; j++) {
			   if (j > 0) {
				   array[i][j] = array[i][j-1] + N3;
			   }

			   std::fill(array[i][j], array[i][j] + N3, 0);
		   }
	    }

	    return array;
	};

	template<
		typename _t>
	void delete3D(
		_t*** array)
	{
	    delete[] array[0][0]; 
	    delete[] array[0];
	    delete[] array;
	};
}
}

//#include <tuple>
//
//namespace prims {
//////std::tuple<unsigned*, vector3*, vector2*> MakePlane(
//////	unsigned xCount,
//////	unsigned zCount)
//////{
//////	// Edge case throws off counts so return nullptrs
//////	if (xCount == 0 || zCount == 0) {
//////		return { nullptr, nullptr, nullptr };
//////	}
//////
//////	// Each count makes two more triangles (6 indices) for every other count
//////	// Each count makes its verts + 1 for the final edge
//////	                                          // vert  # = (2+1)*(1+1) = 6
//////	                                          // index # = 6 * (2)*(1) = 12
//////	                                                  // •------•------•
//////	unsigned indexCount = 6 * xCount * zCount;        // |   /  |   /  |
//////	unsigned vertCount = (xCount + 1) * (zCount + 1); // |  /   |  /   |
//////	                                                  // •------•------•
//////	unsigned* indices = new unsigned[indexCount];
//////	vector3* verts = new vector3[vertCount];
//////	vector2* uvs   = new vector2[vertCount];
//////
//////	// Verts
//////
//////	float xStep = 2.0f / xCount; // This plane is going to span from -1 to 1
//////	float zStep = 2.0f / zCount; // so the step size is 2 / count
//////
//////	float uStep = 1.0f / xCount; // UV space spans from 0 to 1 so step size
//////	float vStep = 1.0f / zCount; // is 1 / count
//////
//////	// Translte origin instead of loop variables
//////
//////	vector3 offset = vector3(-1, 0, -1);
//////
//////	for (unsigned x = 0; x <= xCount; x++) {
//////	for (unsigned z = 0; z <= zCount; z++) {
//////		unsigned i = z + x * (zCount + 1);
//////
//////		// 'Forward' is typically -Z so either the verts or UVs need
//////		// to be flipped to align textures correctly
//////
//////		verts[i] = vector3(x * xStep, 0,          z  * zStep) + offset;
//////		uvs  [i] = vector2(x * uStep,   (zCount - z) * vStep);
//////	}
//////	}
//////
//////	// Index
//////	
//////	//        --------- xCount --------->
//////	//        v          v + 1      (v + 1) + 1
//////	//    |   •------------•------------•
//////	//    |   |          / |          / |
//////	// zCount |       /    |       /    |
//////	//    |   |    /       |    /       |
//////	//    |   | /          | /          |
//////	//    ↓   •------------•------------•
//////	//    v + zc + 1   v + zc + 2   (v + 1) + zc + 2
//////
//////	for (unsigned v = 0, i = 0; v < vertCount - zCount - 2; v++) {
//////		// If at the final row, jump to next column
//////
//////		if ((v + 1) % (zCount + 1) == 0) {
//////			v++;
//////		}
//////
//////		indices[i++] = v;
//////		indices[i++] = v + 1;
//////		indices[i++] = v + zCount + 1;
//////
//////		indices[i++] = v + 1;
//////		indices[i++] = v + zCount + 2;
//////		indices[i++] = v + zCount + 1;
//////	}
//////
//////	return { indices, verts, uvs };
//////}
//////
//////
//////std::tuple<unsigned*, vector3*, vector2*> MakeUvSphere(
//////	unsigned latCount,
//////	unsigned lonCount)
//////{
//////	// Edge case throws off counts so return nullptrs
//////	if (lonCount < 2 || latCount < 2) {
//////		return { nullptr, nullptr, nullptr };
//////	}
//////
//////	// Each longitudinal count makes two triangles (6 indices) for every
//////	// lateral count except for the top and bottom caps, which only make
//////	// one triangle per lon count. Subtracting one from latCount
//////	// effectivly combines the top and bottom counts.
//////
//////	// UV maps require two verts with the same position, but differnt UVs
//////	// so we need counts + 1.
//////
//////	unsigned indexCount = 6 * lonCount  * (latCount - 1);
//////	unsigned vertCount = (lonCount + 1) * (latCount + 1);
//////
//////	unsigned* indices = new unsigned[indexCount];
//////	vector3* verts = new vector3[vertCount];
//////	vector2* uvs   = new vector2[vertCount];
//////
//////	// Verts
//////
//////	float lonStep = Pi2 / lonCount;
//////	float latStep = Pi  / latCount;
//////
//////	for (unsigned lat = 0, v = 0; lat <= latCount; lat++) {
//////	for (unsigned lon = 0;        lon <= lonCount; lon++, v++) {
//////		verts[v] = vector3(                           
//////			cos(lon * lonStep) * sin(lat * latStep), // Circle equations from UVs
//////			cos(lat * latStep - Pi),                 // with Counter-clock wise 
//////			sin(lon * lonStep) * sin(lat * latStep)  // winding
//////		);
//////				
//////		uvs[v] = vector2(           // This is a UV spehre, so UVs map directly
//////			(float)lon / lonCount, // to the verts. This is effectivly a Mercator
//////			(float)lat / latCount  // projection
//////		);                          
//////	}
//////	}
//////
//////	// Index
//////
//////	// Top cap
//////	//
//////	// One triangle connects the first lateral layer to the second per lon. count
//////	// Even though the top points all have the same position, their UVs are different
//////	// so each triangle uses a different point (lon). v = second layer
//////	//
//////	//          -------- lonCount -------->
//////	//                      lon
//////	//    |                  •
//////	//    |                / | \
//////	//    1             /    |    \
//////	//    |          /       |       \
//////	//    |       /                     \
//////	//    V     •------------•------------•
//////	//          v          v + 1      (v + 1) + 1
//////	//
//////
//////	unsigned i = 0;
//////	unsigned v = lonCount + 1;
//////	for (unsigned lon = 0; lon < lonCount; lon++, v++) {
//////		indices[i++] = lon;
//////		indices[i++] = v;
//////		indices[i++] = v + 1;
//////	}
//////
//////	// Middle
//////	//
//////	// Each lateral layer has 2 triangles per lon. count (effectivly making quads)
//////	// Reset v to start of second layer
//////	//
//////	//          -------- lonCount -------->
//////	//          v          v + 1      (v + 1) + 1
//////	//    |     •------------•------------•
//////	//    |     |          / |          / |
//////	// latCount |       /    |       /    |
//////	//    |     |    /       |    /       |
//////	//    |     | /          | /          |
//////	//    ↓     •------------•------------•
//////	//      v + lc + 1   v + lc + 2   (v + 1) + lc + 2
//////	//
//////
//////	v = lonCount + 1;
//////	for (unsigned lat = 1; lat < latCount - 1; lat++, v++) {
//////	for (unsigned lon = 0; lon < lonCount;     lon++, v++) {
//////		indices[i++] = v;
//////		indices[i++] = v + lonCount + 1;
//////		indices[i++] = v + 1;
//////
//////		indices[i++] = v + 1;
//////		indices[i++] = v + lonCount + 1;
//////		indices[i++] = v + lonCount + 2;
//////	}
//////	}
//////
//////	// Bottom cap
//////
//////	// Same as top cap
//////	// Middle loops work out v to correct value
//////	//
//////	//          -------- lonCount -------->
//////	//          v          v + 1      (v + 1) + 1
//////	//    |     •------------•------------•
//////	//    |       \          |          /
//////	//    1          \       |       /
//////	//    |             \    |    /
//////	//    |                \ | /
//////	//    ↓                  •
//////	//                   v + lc + 1
//////	//
//////
//////	for (unsigned lon = 0; lon < lonCount; lon++, v++) {
//////		indices[i++] = v;
//////		indices[i++] = v + lonCount + 1;
//////		indices[i++] = v + 1;
//////	}
//////
//////	return { indices, verts, uvs };
//////}
//////
//////std::tuple<unsigned*, vector3*, vector2*> MakeCapsule(
//////	unsigned resolution,
//////	float height,
//////	float radius)
//////{
//////	// Edge case throws off counts so return nullptrs
//////	if (resolution < 2) {
//////		return { nullptr, nullptr, nullptr };
//////	}
//////
//////	// Almost same generation as UV sphere but we force the
//////	// lat count to be odd so it can be split evenly
//////
//////	unsigned lonCount = resolution;
//////	unsigned latCount = resolution;
//////
//////	if (latCount % 2 == 0) latCount++;
//////
//////	// Each longitudinal count makes two triangles (6 indices) for every
//////	// lateral count except for the top and bottom caps, which only make
//////	// one triangle per lon count. Subtracting one from latCount
//////	// effectivly combines the top and bottom counts.
//////
//////	// UV maps require two verts with the same position, but differnt UVs
//////	// so we need counts + 1.
//////
//////	unsigned indexCount = 6 * lonCount  * (latCount - 1);
//////	unsigned vertCount = (lonCount + 1) * (latCount + 1);
//////
//////	unsigned* indices = new unsigned[indexCount];
//////	vector3* verts = new vector3[vertCount];
//////	vector2* uvs   = new vector2[vertCount];
//////
//////	// Verts
//////
//////	float yOffset = (height - radius * 2) * 0.5;
//////	if (yOffset < 0) yOffset = 0; // Limit yOffset to zero
//////
//////	float lonStep = Pi2  / lonCount;
//////	float latStep = Pi   / latCount;
//////	float xStep   = 1.0f / lonCount;
//////
//////	for (unsigned lat = 0, v = 0; lat <= latCount; lat++) {
//////	for (unsigned lon = 0;        lon <= lonCount; lon++, v++) {
//////		verts[v] = vector3(                           
//////			cos(lon * lonStep) * sin(lat * latStep), // Circle equations from UVs
//////			cos(lat * latStep - Pi),                 // with Counter-clock wise 
//////			sin(lon * lonStep) * sin(lat * latStep)  // winding
//////		);
//////		
//////		verts[v] *= radius;
//////
//////		// Move top hemisphere up and bottom down
//////
//////		if (lat > latCount / 2) verts[v].y += yOffset;
//////		else                    verts[v].y -= yOffset;
//////
//////		// UVs are almost the same as UV sphere, but V needs to be scaled
//////		// to fit the height
//////
//////		uvs[v] = vector2(
//////			(float)lon / lonCount,
//////			(verts[v].y + height) * 0.5f / height
//////		);
//////	}
//////	}
//////
//////	// Index
//////
//////	// Top cap
//////	//
//////	// One triangle connects the first lateral layer to the second per lon. count
//////	// Even though the top points all have the same position, their UVs are different
//////	// so each triangle uses a different point (lon). v = second layer
//////	//
//////	//          -------- lonCount -------->
//////	//                      lon
//////	//    |                  •
//////	//    |                / | \
//////	//    1             /    |    \
//////	//    |          /       |       \
//////	//    |       /                     \
//////	//    ↓     •------------•------------•
//////	//          v          v + 1      (v + 1) + 1
//////	//
//////
//////	unsigned i = 0;
//////	unsigned v = lonCount + 1;
//////	for (unsigned lon = 0; lon < lonCount; lon++, v++) {
//////		indices[i++] = lon;
//////		indices[i++] = v;
//////		indices[i++] = v + 1;
//////	}
//////
//////	// Middle
//////	//
//////	// Each lateral layer has 2 triangles per lon. count (effectivly making quads)
//////	// Reset v to start of second layer
//////	//
//////	//          -------- lonCount -------->
//////	//          v          v + 1      (v + 1) + 1
//////	//    |     •------------•------------•
//////	//    |     |          / |          / |
//////	// latCount |       /    |       /    |
//////	//    |     |    /       |    /       |
//////	//    |     | /          | /          |
//////	//    ↓     •------------•------------•
//////	//      v + lc + 1   v + lc + 2   (v + 1) + lc + 2
//////	//
//////
//////	v = lonCount + 1;
//////	for (unsigned lat = 1; lat < latCount - 1; lat++, v++) {
//////	for (unsigned lon = 0; lon < lonCount;     lon++, v++) {
//////		indices[i++] = v;
//////		indices[i++] = v + lonCount + 1;
//////		indices[i++] = v + 1;
//////
//////		indices[i++] = v + 1;
//////		indices[i++] = v + lonCount + 1;
//////		indices[i++] = v + lonCount + 2;
//////	}
//////	}
//////
//////	// Bottom cap
//////
//////	// Same as top cap
//////	// Middle loops work out v to correct value
//////	//
//////	//          -------- lonCount -------->
//////	//          v          v + 1      (v + 1) + 1
//////	//    |     •------------•------------•
//////	//    |       \          |          /          
//////	//    1          \       |       /
//////	//    |             \    |    /
//////	//    |                \ | /
//////	//    ↓                  •
//////	//                   v + lc + 1
//////	//
//////
//////	for (unsigned lon = 0; lon < lonCount; lon++, v++) {
//////		indices[i++] = v;
//////		indices[i++] = v + lonCount + 1;
//////		indices[i++] = v + 1;
//////	}
//////
//////	return { indices, verts, uvs };
//////}
//////
//////
//////
////
//
//struct pair_hash {
//	template<typename T1, typename T2>
//	size_t operator() (const std::pair<T1, T2>& pair) const {
//		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
//	}
//};
//
//using IndexPair   = std::pair<unsigned, unsigned>;
//using IndexLookup = std::unordered_map<IndexPair, unsigned, pair_hash>;
//
//#define ICO_X .525731112119133606f
//#define ICO_Z .850650808352039932f
//
//static const unsigned IcoVertCount  = 12;
//static const unsigned IcoIndexCount = 60;
//
//static const vector3 IcoVerts[] = {
//	vector3(     0, -ICO_X, -ICO_Z),
//	vector3(-ICO_X, -ICO_Z,      0),
//	vector3( ICO_Z,      0, -ICO_X),
//	vector3( ICO_X, -ICO_Z,      0),
//	vector3( ICO_X,  ICO_Z,      0),
//	vector3(-ICO_X, -ICO_Z,      0),
//	vector3( ICO_Z,      0,  ICO_X),
//	vector3(     0, -ICO_X,  ICO_Z),
//	vector3( ICO_X,  ICO_Z,      0),
//	vector3(-ICO_X, -ICO_Z,      0),
//	vector3(     0,  ICO_X,  ICO_Z),
//	vector3(-ICO_Z,      0,  ICO_X),
//	vector3( ICO_X,  ICO_Z,      0),
//	vector3(-ICO_X, -ICO_Z,      0),
//	vector3(-ICO_X,  ICO_Z,      0),
//	vector3(-ICO_Z,      0, -ICO_X),
//	vector3( ICO_X,  ICO_Z,      0),
//	vector3(-ICO_X, -ICO_Z,      0),
//	vector3(    0,   ICO_X, -ICO_Z),
//	vector3(    0,  -ICO_X, -ICO_Z),
//	vector3( ICO_X,  ICO_Z,      0),
//	vector3( ICO_Z,      0, -ICO_X)
//};
//
//static const vector3 IcoUvs[] = {
//	vector2(0.0,      0.157461),
//	vector2(0.090909, 0.0),
//	vector2(0.090909, 0.314921),  //
//	vector2(0.181818, 0.157461),  // Verts & UVs are ordered by U then Y coords,
//	vector2(0.181818, 0.472382),  //
//	vector2(0.272727, 0.0),       //      4   8   C   G   K
//	vector2(0.272727, 0.314921),  //     / \ / \ / \ / \ / \ 
//	vector2(0.363636, 0.157461),  //    2---6---A---E---I---L
//	vector2(0.363636, 0.472382),  //   / \ / \ / \ / \ / \ /
//	vector2(0.454545, 0.0),       //  0---3---7---B---F---J
//	vector2(0.454545, 0.314921),  //   \ / \ / \ / \ / \ /
//	vector2(0.545454, 0.157461),  //    1   5   9   D	  H
//	vector2(0.545454, 0.472382),  //
//	vector2(0.636363, 0.0),       // [4, 8, C, G, K] have the same position vert
//	vector2(0.636363, 0.314921),  // [1, 5, 9, D, H] have the same position vert
//	vector2(0.727272, 0.157461),  // [0, J]          have the same position vert
//	vector2(0.727272, 0.472382),  // [2, L]          have the same position vert
//	vector2(0.818181, 0.0),       // 
//	vector2(0.818181, 0.314921),
//	vector2(0.90909,  0.157461),
//	vector2(0.90909,  0.472382),
//	vector2(1.0,      0.314921)
//};
//
//static const unsigned IcoIndex[] = {
//	 0,  1,  3, // Bottom
//	 3,  5,  7,
//	 7,  9, 11,
//	11, 13, 15,
//	15, 17, 19,
//	 0,  3,  2, // Middle
//	 2,  3,  6,
//	 3,  7,  6,
//	 6,  7, 10,
//	 7, 11, 10,
//	10, 11, 14,
//	11, 15, 14,
//	14, 15, 18,
//	15, 19, 18,
//	18, 19, 21,
//	 2,  6,  4, // Top
//	 6, 10,  8,
//	10, 14, 12,
//	14, 18, 16,
//	18, 21, 20
//};
//
//std::tuple<unsigned*, vector3*, vector2*> MakeIcosphere(
//	unsigned resolution)
//{
//	// For every resolution, each face of the initial mesh gets split into 4 triangles.
//	// Most of the vertices end up begin shared between many differnt triangles
//	// so the number of vertices works out to a geometric sequence summing 4^R
//	//
//	//     •-------•               •---•---•
//	//    / \     /               / \ / \ /
//	//   /   \   /	  ------->    •---•---•
//	//  /     \ /               / \ / \ /
//	// •-------•               •---•---•
//
//	unsigned res = (unsigned)pow(4, resolution);
//
//	unsigned indexCount = 60 * res;
//	unsigned vertCount = 22 + 60 * (1 - res) / -3;
//
//	unsigned* indices = new unsigned[indexCount];
//	vector3* verts = new vector3[vertCount];
//	vector2* uvs   = new vector2[vertCount];
//
//	memcpy(indices, IcoIndex, IcoIndexCount * sizeof(unsigned));
//	memcpy(verts, IcoVerts, IcoVertCount * sizeof(vector3));
//	memcpy(verts, IcoUvs,   IcoVertCount * sizeof(vector2));
//
//	// Verts & Index
//
//	unsigned currentIndexCount = IcoIndexCount;
//	unsigned currentVertCount = IcoVertCount;
//	for (unsigned i = 0; i < resolution; i++) {
//		SubDevideVerts(verts, uvs, indices, currentIndexCount, currentVertCount);
//	}
//
//	// At this point we have a tessellated icosahedron, but most of the points don't
//	// have length = 0. Normalizing them it will push all but the initial points out
//	// creating a sphere 
//
//	for (unsigned i = 0; i < vertCount; i++) {
//		verts[i].normalize();
//	}
//
//	return { indices, verts, uvs };
//}
//
//void SubDevideVerts(
//	vector3* verts,
//	vector2* uvs,
//	unsigned* index,
//	unsigned& currentIndexCount,
//	unsigned& currentVertCount)
//{
//	IndexLookup lookup;
//
//	// Next index is going to be the current index * the 16 new indices seen below
//
//	unsigned* next = new unsigned[16 * currentIndexCount];
//	unsigned nextCount = 0;
//
//	//       i + 2                    i + 2
//	//        / \                      / \
//	//       /   \                    /   \
//	//      /     \                  /     \
//	//     /       \    ------->   mid2---mid1
//	//    /         \              / \     / \
//	//   /           \            /   \   /   \
//	//  /             \          /     \ /     \
//	// i-------------i + 1      i-----mid0----i + 1
//
//	for (unsigned i = 0; i < currentIndexCount; i += 3) {
//		unsigned mid0 = CreateVertexForEdge(
//			lookup, verts, uvs, index[i],     index[i + ((i + 1) % 3)], currentVertCount);
//
//		unsigned mid1 = CreateVertexForEdge(
//			lookup, verts, uvs, index[i + 1], index[i + ((i + 2) % 3)], currentVertCount);
//
//		unsigned mid2 = CreateVertexForEdge(
//			lookup, verts, uvs, index[i + 2], index[i + ((i + 3) % 3)], currentVertCount);
//
//		next[nextCount++] = index[i];     next[nextCount++] = mid0; next[nextCount++] = mid2;
//		next[nextCount++] = index[i + 1]; next[nextCount++] = mid1; next[nextCount++] = mid0;
//		next[nextCount++] = index[i + 2]; next[nextCount++] = mid2; next[nextCount++] = mid1;
//		next[nextCount++] = mid0;         next[nextCount++] = mid1; next[nextCount++] = mid2;
//	}
//
//	// Update index with new index
//
//	memcpy(index, next, nextCount * sizeof(unsigned));
//	currentIndexCount = nextCount;
//
//	delete[] next;
//}
//
//unsigned CreateVertexForEdge(
//	IndexLookup& lookup,
//	vector3* verts,
//	vector2* uvs, 
//	unsigned first,
//	unsigned second,
//	unsigned& currentVertCount)
//{
//	// Order pair by smallest index first because first, second is equivalient
//	// to second, first but the hash is not
//	IndexPair edge = first < second ? IndexPair(first, second) : IndexPair(second, first);
//
//	// Check the edge for a vert
//	auto inserted = lookup.insert({ edge, currentVertCount });
//
//	// If there wasn't one, create a new vert in the middle of the edge
//	if (inserted.second) {
//		if(verts) verts[currentVertCount  ] = (verts[first] + verts[second]) / 2;
//		if (uvs)  uvs  [currentVertCount++] = (uvs  [first] + uvs  [second]) / 2;
//	}
//
//	// Return the index of the vert
//	return inserted.first->second;
//}

//}

	using namespace Graphics;
}
