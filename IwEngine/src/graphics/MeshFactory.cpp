#include "iw/graphics/MeshFactory.h"

#define ICO_X .525731112119133606f
#define ICO_Z .850650808352039932f

namespace iw {
namespace Graphics {
	// Icosphere

	static const unsigned IcoVertCount  = 12;
	static const unsigned IcoIndexCount = 60;

	static const iw::vector3 IcoVerts[] = {
		iw::vector3(-ICO_X,  0,	     ICO_Z),
		iw::vector3( ICO_X,  0,	     ICO_Z),
		iw::vector3(-ICO_X,  0,     -ICO_Z),
		iw::vector3( ICO_X,  0,     -ICO_Z),
		iw::vector3(0,		 ICO_Z,  ICO_X),
		iw::vector3(0,		 ICO_Z, -ICO_X),
		iw::vector3(0,      -ICO_Z,  ICO_X),
		iw::vector3(0,	    -ICO_Z, -ICO_X),
		iw::vector3( ICO_Z,  ICO_X,  0),
		iw::vector3(-ICO_Z,  ICO_X,  0),
		iw::vector3( ICO_Z, -ICO_X,  0),
		iw::vector3(-ICO_Z, -ICO_X,  0)
	};

	static const unsigned IcoIndex[] = {
		 1,  4,  0,
		 4,  9,	 0,
		 4,  5,	 9,
		 8,  5,	 4,
		 1,  8,	 4,
		 1, 10,	 8,
		10,  3,	 8,
		 8,  3,	 5,
		 3,  2,	 5,
		 3,  7,	 2,
		 3, 10,	 7,
		10,  6,	 7,
		 6, 11,	 7,
		 6,  0, 11,
		 6,  1,	 0,
		10,  1,	 6,
		11,  0,	 9,
		 2, 11,	 9,
		 5,  2,	 9,
		11,  2,	 7
	};

	// Triangle

	static const unsigned TriVertCount  = 4;
	static const unsigned TriUvCount    = 4;
	static const unsigned TriIndexCount = 12;

	static const iw::vector3 TriVerts[] = {
		iw::vector3(cos(iw::Pi2 * 0 / 3), -1, sin(iw::Pi2 * 0 / 3)),
		iw::vector3(cos(iw::Pi2 * 1 / 3), -1, sin(iw::Pi2 * 1 / 3)),
		iw::vector3(cos(iw::Pi2 * 2 / 3), -1, sin(iw::Pi2 * 2 / 3)),
		iw::vector3(0, 1, 0),
	};

	static const iw::vector2 TriUvs[] = {
		iw::vector2(0,    0),
		iw::vector2(1,    0),
		iw::vector2(0.5f, 1),
		iw::vector2(1,    1),
	};

	static const unsigned TriIndex[] = {
		0, 1, 2,
		1, 0, 3,
		2, 1, 3,
		0, 2, 3
	};

	Mesh* MakeIcosphere(
		unsigned resolution)
	{
		unsigned res = (unsigned)pow(4, resolution);

		unsigned vertCount  = 12 + (30 * res);
		unsigned indexCount = 60 * res;

		iw::vector3* verts   = new iw::vector3[vertCount];
		unsigned*    indices = new unsigned   [indexCount];

		memcpy(verts,   IcoVerts, IcoVertCount  * sizeof(iw::vector3));
		memcpy(indices, IcoIndex, IcoIndexCount * sizeof(unsigned));

		// Verts & Index

		unsigned currentIndexCount = IcoIndexCount;
		unsigned currentVertCount  = IcoVertCount;
		for (unsigned i = 0; i < resolution; i++) {
			detail::SubDevideVerts(verts, indices, currentIndexCount, currentVertCount);
		}

		for (unsigned i = 0; i < vertCount; i++) {
			verts[i].normalize();
		}

		Mesh* mesh = new Mesh();
		mesh->SetVertices(vertCount,  verts);
		mesh->SetIndices (indexCount, indices);
		mesh->GenNormals();

		delete[] verts;
		delete[] indices;

		return mesh;
	}

	Mesh* MakeUvSphere(
		unsigned latCount,
		unsigned lonCount)
	{
		unsigned vertCount  = (latCount + 1) * (lonCount + 1);
		unsigned indexCount = 6 * (lonCount + (latCount - 2) * lonCount);

		// invalid counts
		if (indexCount == 0) {
			return nullptr;
		}

		iw::vector3* verts   = new iw::vector3[vertCount];
		iw::vector3* norms   = new iw::vector3[vertCount];
		iw::vector2* uvs     = new iw::vector2[vertCount];
		unsigned*    indices = new unsigned   [indexCount];

		// Verts

		float lonStep = iw::Pi2 / lonCount;
		float latStep = iw::Pi  / latCount;

		size_t vert = 0;
		for (unsigned lat = 0; lat <= latCount; lat++) {
			for (unsigned lon = 0; lon <= lonCount; lon++) {
				float y = sin(-iw::Pi / 2 + lat * latStep);
				float x = cos(lon * lonStep) * sin(lat * latStep);
				float z = sin(lon * lonStep) * sin(lat * latStep);

				float u = (float)lon / lonCount;
				float v = (float)lat / latCount;

				verts[vert] = iw::vector3(x, y, z);
				norms[vert] = iw::vector3(x, y, z);
				uvs  [vert] = iw::vector2(u, v);

				vert++;
			}
		}

		// Index

		unsigned i = 0;
		unsigned v = lonCount + 1;
		for (unsigned lon = 0; lon < lonCount; ++lon, v++) {
			indices[i++] = lon;
			indices[i++] = v;
			indices[i++] = v + 1;
		}

		for (unsigned lat = 1; lat < latCount - 1; lat++) {
			v = lat * (lonCount + 1);
			for (unsigned lon = 0; lon < lonCount; lon++, v++) {
				indices[i++] = v;
				indices[i++] = v + lonCount + 1;
				indices[i++] = v + 1;
				indices[i++] = v + 1;
				indices[i++] = v + lonCount + 1;
				indices[i++] = v + lonCount + 2;
			}
		}

		v = (latCount - 1) * (lonCount + 1);
		for (unsigned lon = 0; lon < lonCount; lon++, v++) {
			indices[i++] = v;
			indices[i++] = v + lonCount + 1;
			indices[i++] = v + 1;
		}

		Mesh* mesh = new Mesh();
		mesh->SetVertices(vertCount,  verts);
		mesh->SetNormals (vertCount,  norms);
		mesh->SetUVs     (vertCount,  uvs);
		mesh->SetIndices (indexCount, indices);

		delete[] verts;
		delete[] norms;
		delete[] uvs;
		delete[] indices;

		return mesh;
	}

	Mesh* MakeCapsule(
		unsigned resolution)
	{
		const float height = 4.0f;
		const float radius = 1.0f;

		// make segments an even number
		if (resolution % 2 != 0)
			resolution++;

		// extra vertex on the seam
		int points = resolution + 1;

		unsigned vertCount  = points * (points + 1);
		unsigned indexCount = (resolution * (resolution + 1) * 2 * 3);

		vector3*  vertices = new vector3 [vertCount];
		vector2*  uvs      = new vector2 [vertCount];
		unsigned* indices  = new unsigned[indexCount];


		// calculate points around a circle
		float* pX = new float[points];
		float* pZ = new float[points];
		float* pY = new float[points];
		float* pR = new float[points];

		float calcH = 0.0f;
		float calcV = 0.0f;

		for (int i = 0; i < points; i++) {
			pX[i] = sin(calcH);
			pZ[i] = cos(calcH);
			pY[i] = cos(calcV);
			pR[i] = sin(calcV);

			calcH += 2 * Pi / resolution;
			calcV +=     Pi / resolution;
		}
		
		float yOff = (height - radius * 2.0f) * 0.5f;
		if (yOff < 0) {
			yOff = 0;
		}

		float stepX = 1.0f / (points - 1);
		int   top   = ceil (points * 0.5f);
		int   btm   = floor(points * 0.5f);

		int v = 0;

		// Top Hemisphere

		for (int y = 0; y < top; y++) {
			for (int x = 0; x < points; x++) {
				vertices[v] = vector3(
					pX[x] * pR[y], 
					pY[y] + yOff, 
					pZ[x] * pR[y]
				);

				uvs[v] = vector2(
					1.0f - stepX * x, 
					(vertices[v].y + (height * 0.5f)) / height
				);

				v++;
			}
		}

		// Bottom Hemisphere

		for (int y = btm; y < points; y++) {
			for (int x = 0; x < points; x++) {
				vertices[v] = vector3(
					pX[x] * pR[y], 
					pY[y] - yOff, 
					pZ[x] * pR[y]) * radius;

				uvs[v] = vector2(
					1.0f - stepX * x, 
					(vertices[v].y + (height * 0.5f)) / height
				);

				v++;
			}
		}

		// Index

		unsigned i = 0;
		for (int y = 0; y < resolution + 1; y++) {
			for (int x = 0; x < resolution; x++) {
				indices[i++] = (y + 0) * points + x + 0;
				indices[i++] = (y + 1) * points + x + 0;
				indices[i++] = (y + 1) * points + x + 1;

				indices[i++] = (y + 0) * points + x + 1;
				indices[i++] = (y + 0) * points + x + 0;
				indices[i++] = (y + 1) * points + x + 1;
			}
		}

		Mesh* mesh = new Mesh();

		mesh->SetVertices(vertCount, vertices);
		mesh->SetUVs     (vertCount, uvs);
		mesh->SetIndices (indexCount, indices);

		mesh->GenNormals();

		delete[] pX;
		delete[] pZ;
		delete[] pY;
		delete[] pR;
		delete[] vertices;
		delete[] uvs;
		delete[] indices;

		return mesh;
	}

	Mesh* MakeTetrahedron(
		unsigned int resolution)
	{
		unsigned res = (unsigned)pow(4, resolution);

		unsigned vertCount  = 4 + (6 * res);
		unsigned indexCount = 12 * res;

		iw::vector3* verts   = new iw::vector3[vertCount];
		iw::vector2* uvs     = new iw::vector2[vertCount];
		unsigned*    indices = new unsigned   [indexCount];

		memcpy(verts,   TriVerts, TriVertCount  * sizeof(iw::vector3));
		memcpy(uvs,     TriUvs,   TriUvCount    * sizeof(iw::vector2));
		memcpy(indices, TriIndex, TriIndexCount * sizeof(unsigned));

		// Verts & Index

		unsigned currentIndexCount = TriIndexCount;
		unsigned currentUvCount    = TriUvCount;
		unsigned currentVertCount  = TriVertCount;
		for (unsigned i = 0; i < resolution; i++) {
			detail::SubDevideUvs  (uvs,   indices, currentIndexCount, currentUvCount);
			detail::SubDevideVerts(verts, indices, currentIndexCount, currentVertCount);
		}

		Mesh* mesh = new Mesh();
		mesh->SetVertices(vertCount,  verts);
		mesh->SetUVs     (vertCount,  uvs);
		mesh->SetIndices (indexCount, indices);
		mesh->GenNormals (false);

		delete[] verts;
		delete[] uvs;
		delete[] indices;

		return mesh;
	}

	IWGRAPHICS_API
	Mesh* MakePlane(
		unsigned xCount,
		unsigned zCount)
	{
		unsigned vertCount  = (xCount + 1) * (zCount + 1);
		unsigned indexCount = 6 * xCount * zCount;

		iw::vector3* verts   = new iw::vector3[vertCount];
		iw::vector3* norms   = new iw::vector3[vertCount];
		iw::vector2* uvs     = new iw::vector2[vertCount];
		unsigned*    indices = new unsigned   [indexCount];

		float stepX = 2.0f / xCount;
		float stepZ = 2.0f / zCount;

		float stepU = 2.0f / xCount;
		float stepV = 2.0f / zCount;

		iw::vector3 offset = -(iw::vector3::unit_x + iw::vector3::unit_z);

		for (unsigned x = 0; x <= xCount; x++) {
			for (unsigned z = 0; z <= zCount; z++) {
				unsigned i = z + x * (zCount + 1);

				verts[i] = offset + iw::vector3(x * stepX, 0, z * stepZ);
				norms[i] = iw::vector3::unit_y;
				uvs  [i] = iw::vector2(x * stepU, z * stepV);
			}
		}

		unsigned i = 0, v = 0;
		while(v <= vertCount - (zCount + 3)) {
			indices[i++] = v;
			indices[i++] = v + 1; 
			indices[i++] = v + zCount + 1;

			indices[i++] = v + 1;
			indices[i++] = v + zCount + 2; 
			indices[i++] = v + zCount + 1;

			v++;
			if (   v != 0
				&& (v + 1) % (zCount + 1) == 0)
			{
				v++;
			}
		}

		Mesh* mesh = new Mesh();
		mesh->SetVertices(vertCount,  verts);
		mesh->SetNormals (vertCount,  norms);
		mesh->SetUVs     (vertCount,  uvs);
		mesh->SetIndices (indexCount, indices);

		delete[] verts;
		delete[] norms;
		delete[] uvs;
		delete[] indices;

		return mesh;
	}

namespace detail {
	void SubDevideVerts(
		iw::vector3* verts,
		unsigned* index,
		unsigned& currentIndexCount,
		unsigned& currentVertCount)
	{
		IndexLookup lookup;
		unsigned* next_index = new unsigned[size_t(16U) * currentIndexCount];
		unsigned nextIndexCount = 0;

		unsigned indexCount = currentIndexCount;
		for (unsigned i = 0; i < indexCount; i += 3) {
			unsigned mid0 = CreateVertexForEdge(lookup, verts, index[i],     index[i + ((i + 1) % 3)], currentVertCount);
			unsigned mid1 = CreateVertexForEdge(lookup, verts, index[i + 1], index[i + ((i + 2) % 3)], currentVertCount);
			unsigned mid2 = CreateVertexForEdge(lookup, verts, index[i + 2], index[i + ((i + 3) % 3)], currentVertCount);

			next_index[nextIndexCount++] = index[i];	 next_index[nextIndexCount++] = mid0; next_index[nextIndexCount++] = mid2;
			next_index[nextIndexCount++] = index[i + 1]; next_index[nextIndexCount++] = mid1; next_index[nextIndexCount++] = mid0;
			next_index[nextIndexCount++] = index[i + 2]; next_index[nextIndexCount++] = mid2; next_index[nextIndexCount++] = mid1;
			next_index[nextIndexCount++] = mid0;	     next_index[nextIndexCount++] = mid1; next_index[nextIndexCount++] = mid2;
		}

		memcpy(index, next_index, nextIndexCount * sizeof(unsigned));
		currentIndexCount = nextIndexCount;

		delete[] next_index;
	}


	/*
	    0, 1, 2,
		1, 0, 3,
		2, 1, 3,
		0, 2, 3
	*/

	void SubDevideUvs(
		iw::vector2* uvs,
		const unsigned* index,
		unsigned indexCount,
		unsigned& currentUvCount)
	{
		IndexLookup lookup;
		for (unsigned i = 0; i < indexCount; i += 3) {
			CreateUvsForEdge(lookup, uvs, index[i],     index[i + ((i + 1) % 3)], currentUvCount);
			CreateUvsForEdge(lookup, uvs, index[i + 1], index[i + ((i + 2) % 3)], currentUvCount);
			CreateUvsForEdge(lookup, uvs, index[i + 2], index[i + ((i + 3) % 3)], currentUvCount);
		}
	}

	unsigned CreateVertexForEdge(
		IndexLookup& lookup,
		iw::vector3* verts,
		unsigned first,
		unsigned second,
		unsigned& currentVertCount)
	{
		IndexPair key = first < second ? IndexPair(first, second) : IndexPair(second, first);

		auto inserted = lookup.insert({ key, currentVertCount });
		if (inserted.second) {
			auto& edge0 = verts[first];
			auto& edge1 = verts[second];
			auto point = (edge0 + edge1) / 2;
			verts[currentVertCount++] = point;
		}

		return inserted.first->second;
	}

	unsigned CreateUvsForEdge(
		IndexLookup& lookup,
		iw::vector2* uvs,
		unsigned first,
		unsigned second,
		unsigned& currentUvCount)
	{
		IndexPair key = first < second ? IndexPair(first, second) : IndexPair(second, first);

		auto inserted = lookup.insert({ key, currentUvCount });
		if (inserted.second) {
			auto& edge0 = uvs[first];
			auto& edge1 = uvs[second];
			auto point = (edge0 + edge1) / 2;
			uvs[currentUvCount++] = point;
		}

		return inserted.first->second;
	}
}
}
}
