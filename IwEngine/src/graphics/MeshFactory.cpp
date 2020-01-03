#include "iw/graphics/MeshFactory.h"

#define ICO_X .525731112119133606f
#define ICO_Z .850650808352039932f

namespace IW {
namespace Graphics {
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

	static const unsigned TriVertCount = 4;
	static const unsigned TriIndexCount = 12;

	static const iw::vector3 TriVerts[] = {
		iw::vector3(cos(iw::PI2 * 0 / 3), -1, sin(iw::PI2 * 0 / 3)) * 2,
		iw::vector3(cos(iw::PI2 * 1 / 3), -1, sin(iw::PI2 * 1 / 3)) * 2,
		iw::vector3(cos(iw::PI2 * 2 / 3), -1, sin(iw::PI2 * 2 / 3)) * 2,
		iw::vector3(0, 1, 0),
	};

	static const unsigned TriIndex[] = {
		1, 0, 3,
		1, 0, 3,
		2, 1, 3,
		0, 2, 3
	};


	Mesh* MakeIcosphere(
		unsigned resolution)
	{
		unsigned res = (unsigned)pow(4, resolution);

		Mesh* mesh = new Mesh();
		mesh->VertexCount = 12 + (30 * res);
		mesh->IndexCount  = 60 * res;
		mesh->Vertices = new iw::vector3[mesh->VertexCount];
		mesh->Normals  = new iw::vector3[mesh->VertexCount];
		mesh->Indices  = new unsigned[mesh->IndexCount];

		memcpy(mesh->Vertices, IcoVerts, IcoVertCount * sizeof(iw::vector3));
		memcpy(mesh->Indices, IcoIndex, IcoIndexCount * sizeof(unsigned));

		// Verts & Index

		unsigned indexCount = IcoIndexCount;
		unsigned vertCount  = IcoVertCount;
		for (unsigned i = 0; i < resolution; i++) {
			detail::SubDevide(mesh->Vertices, mesh->Indices, indexCount, vertCount);
		}

		for (unsigned i = 0; i < mesh->VertexCount; i++) {
			mesh->Vertices[i].normalize();
		}

		// Normals

		mesh->GenNormals();

		return mesh;
	}

	Mesh* MakeUvSphere(
		unsigned latCount,
		unsigned lonCount)
	{
		Mesh* mesh = new Mesh();
		mesh->VertexCount = (latCount + 1) * (lonCount + 1);
		mesh->IndexCount  = 6 * (lonCount + (latCount - 2) * lonCount);
		mesh->Vertices = new iw::vector3[mesh->VertexCount];
		mesh->Normals  = new iw::vector3[mesh->VertexCount];
		mesh->Uvs      = new iw::vector2[mesh->VertexCount];
		mesh->Indices  = new unsigned   [mesh->IndexCount];

		// Verts

		float lonStep = iw::PI2 / lonCount;
		float latStep = iw::PI  / latCount;

		size_t vert = 0;
		for (unsigned lat = 0; lat <= latCount; lat++) {
			for (unsigned lon = 0; lon <= lonCount; lon++) {
				float y = sin(-iw::PI / 2 + lat * latStep);
				float x = cos(lon * lonStep) * sin(lat * latStep);
				float z = sin(lon * lonStep) * sin(lat * latStep);

				float u = (float)lon / lonCount;
				float v = (float)lat / latCount;

				mesh->Vertices[vert] = iw::vector3(x, y, z);
				mesh->Normals [vert] = iw::vector3(x, y, z);
				mesh->Uvs     [vert] = iw::vector2(u, v);

				vert++;
			}
		}

		// Index

		size_t index = 0;
		unsigned v = lonCount + 1;
		for (unsigned lon = 0; lon < lonCount; ++lon, v++) {
			mesh->Indices[index++] = lon;
			mesh->Indices[index++] = v;
			mesh->Indices[index++] = v + 1;
		}

		for (unsigned lat = 1; lat < latCount - 1; lat++) {
			v = lat * (lonCount + 1);
			for (unsigned lon = 0; lon < lonCount; lon++, v++) {
				mesh->Indices[index++] = v;
				mesh->Indices[index++] = v + lonCount + 1;
				mesh->Indices[index++] = v + 1;
				mesh->Indices[index++] = v + 1;
				mesh->Indices[index++] = v + lonCount + 1;
				mesh->Indices[index++] = v + lonCount + 2;
			}
		}

		v = (latCount - 1) * (lonCount + 1);
		for (unsigned lon = 0; lon < lonCount; lon++, v++) {
			mesh->Indices[index++] = v;
			mesh->Indices[index++] = v + lonCount + 1;
			mesh->Indices[index++] = v + 1;
		}

		return mesh;
	}

	Mesh* MakeTetrahedron(
		unsigned int resolution)
	{
		unsigned res = (unsigned)pow(4, resolution);

		Mesh* mesh = new Mesh();
		mesh->VertexCount = 4 + (6 * res);
		mesh->IndexCount  = 12 * res;
		mesh->Vertices = new iw::vector3[mesh->VertexCount];
		mesh->Normals  = new iw::vector3[mesh->VertexCount];
		mesh->Indices  = new unsigned   [mesh->IndexCount];

		memcpy(mesh->Vertices, TriVerts, TriVertCount * sizeof(iw::vector3));
		memcpy(mesh->Indices, TriIndex, TriIndexCount * sizeof(unsigned));

		// Verts & Index

		unsigned indexCount = TriIndexCount;
		unsigned vertCount  = TriVertCount;
		for (unsigned i = 0; i < resolution; i++) {
			detail::SubDevide(mesh->Vertices, mesh->Indices, indexCount, vertCount);
		}

		// Normals

		mesh->GenNormals();

		return mesh;
	}

	IWGRAPHICS_API
	Mesh* MakePlane(
		unsigned xCount,
		unsigned zCount)
	{
		Mesh* mesh = new Mesh();
		mesh->VertexCount = (xCount + 1) * (zCount + 1);
		mesh->IndexCount  = 6 * xCount * zCount;
		mesh->Vertices = new iw::vector3[mesh->VertexCount];
		mesh->Normals  = new iw::vector3[mesh->VertexCount];
		mesh->Uvs      = new iw::vector2[mesh->VertexCount];
		mesh->Indices  = new unsigned   [mesh->IndexCount];

		float stepX = 1.0f / xCount;
		float stepZ = 1.0f / zCount;

		float stepU = 1.0f / xCount;
		float stepV = 1.0f / zCount;

		iw::vector3 offset = (-iw::vector3::unit_x - iw::vector3::unit_z) / 2;

		for (unsigned x = 0; x <= xCount; x++) {
			for (unsigned z = 0; z <= zCount; z++) {
				unsigned i = z + x * (zCount + 1);

				mesh->Vertices[i] = offset + iw::vector3(x * stepX, 0, z * stepZ);
				mesh->Normals[i]  = iw::vector3::unit_y;
				mesh->Uvs[i]      = iw::vector2(x * stepU, z * stepV);
			}
		}

		unsigned i = 0, v = 0;
		while(v <= mesh->VertexCount - (zCount + 3)) {
			mesh->Indices[i++] = v;
			mesh->Indices[i++] = v + 1;
			mesh->Indices[i++] = v + zCount + 1;

			mesh->Indices[i++] = v + 1;
			mesh->Indices[i++] = v + zCount + 2;
			mesh->Indices[i++] = v + zCount + 1;

			v++;
			if (   v != 0
				&& (v + 1) % (zCount + 1) == 0)
			{
				v++;
			}
		}

		return mesh;
	}

namespace detail {
	void SubDevide(
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
}
}
}
