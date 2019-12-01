#include "iw/graphics/MeshFactory.h"
#define ICO_X .525731112119133606f
#define ICO_Z .850650808352039932f

namespace IW {
	const unsigned mesh_factory::ico_vert_count  = 12;
	const unsigned mesh_factory::ico_index_count = 60;

	static const iw::vector3 ico_vert_data[] = {
		iw::vector3(-ICO_X,  0,	    ICO_Z),
		iw::vector3(ICO_X,  0,	    ICO_Z),
		iw::vector3(-ICO_X,  0,     -ICO_Z),
		iw::vector3(ICO_X,  0,     -ICO_Z),
		iw::vector3(0,		 ICO_Z,  ICO_X),
		iw::vector3(0,		 ICO_Z, -ICO_X),
		iw::vector3(0,     -ICO_Z,  ICO_X),
		iw::vector3(0,	    -ICO_Z, -ICO_X),
		iw::vector3(ICO_Z,  ICO_X,  0),
		iw::vector3(-ICO_Z,  ICO_X,  0),
		iw::vector3(ICO_Z, -ICO_X,  0),
		iw::vector3(-ICO_Z, -ICO_X,  0)
	};

	static const unsigned ico_index_data[] = {
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

	// clock wise
	//static const unsigned ico_index_data[] = {
	//	0,  4,  1,
	//	0,  9,  4,
	//	9,  5,  4,
	//	4,  5,  8,
	//	4,  8,  1,
	//	8, 10,  1,
	//	8,  3, 10,
	//	5,  3,  8,
	//	5,  2,  3,
	//	2,  7,  3,
	//	7, 10,  3,
	//	7,  6, 10,
	//	7, 11,  6,
	//	11,  0,  6,
	//	0,  1,  6,
	//	6,  1, 10,
	//	9,  0, 11,
	//	9, 11,  2,
	//	9,  2,  5,
	//	7,  2, 11
	//};

	const iw::vector3* mesh_factory::ico_verts = ico_vert_data;
	const unsigned* mesh_factory::ico_index = ico_index_data;

	Mesh* mesh_factory::create_icosphere(
		unsigned resolution)
	{
		unsigned res = (unsigned)pow(4, resolution);

		Mesh* mesh = new Mesh();
		mesh->VertexCount = 12 + (30 * res);
		mesh->IndexCount  = 60 * res;
		mesh->Vertices = new iw::vector3[mesh->VertexCount];
		mesh->Normals  = new iw::vector3[mesh->VertexCount];
		mesh->Indices  = new unsigned[mesh->IndexCount];

		memcpy(mesh->Vertices, ico_verts, ico_vert_count  * sizeof(iw::vector3));
		memcpy(mesh->Indices,  ico_index, ico_index_count * sizeof(unsigned));

		// Verts & Index

		unsigned current_index_count = ico_index_count;
		unsigned current_vert_count  = ico_vert_count;
		for (unsigned i = 0; i < resolution; i++) {
			sub_devide(mesh->Vertices, mesh->Indices, current_index_count, current_vert_count);
		}

		// Normals

		mesh->GenNormals();

		return mesh;
	}

	Mesh* mesh_factory::create_uvsphere(
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

	void mesh_factory::sub_devide(
		iw::vector3* verts,
		unsigned* index,
		unsigned& current_index_count,
		unsigned& current_vert_count)
	{
		index_lookup_map lookup;
		unsigned* next_index = new unsigned[16U * current_index_count];
		unsigned next_index_count = 0;

		unsigned index_count = current_index_count;
		for (unsigned i = 0; i < index_count; i += 3) {
			unsigned mid0 = create_vertex_for_edge(lookup, verts, index[i],     index[i + ((i + 1) % 3)], current_vert_count);
			unsigned mid1 = create_vertex_for_edge(lookup, verts, index[i + 1], index[i + ((i + 2) % 3)], current_vert_count);
			unsigned mid2 = create_vertex_for_edge(lookup, verts, index[i + 2], index[i + ((i + 3) % 3)], current_vert_count);

			next_index[next_index_count++] = index[i];	   next_index[next_index_count++] = mid0; next_index[next_index_count++] = mid2;
			next_index[next_index_count++] = index[i + 1]; next_index[next_index_count++] = mid1; next_index[next_index_count++] = mid0;
			next_index[next_index_count++] = index[i + 2]; next_index[next_index_count++] = mid2; next_index[next_index_count++] = mid1;
			next_index[next_index_count++] = mid0;	       next_index[next_index_count++] = mid1; next_index[next_index_count++] = mid2;
		}

		memcpy(index, next_index, next_index_count * sizeof(unsigned));
		current_index_count = next_index_count;

		delete[] next_index;
	}

	unsigned mesh_factory::create_vertex_for_edge(
		index_lookup_map& lookup,
		iw::vector3* verts,
		unsigned first,
		unsigned second,
		unsigned& current_vert_count)
	{
		index_pair key = first < second ? index_pair(first, second) : index_pair(second, first);

		auto inserted = lookup.insert({ key, current_vert_count });
		if (inserted.second) {
			auto& edge0 = verts[first];
			auto& edge1 = verts[second];
			auto point = (edge0 + edge1).normalized_fast();
			verts[current_vert_count++] = point;
		}

		return inserted.first->second;
	}
}
