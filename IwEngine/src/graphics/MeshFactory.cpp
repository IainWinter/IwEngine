#include "iw/graphics/MeshFactory.h"
#define ICO_X .525731112119133606f
#define ICO_Z .850650808352039932f

namespace IW {
	const size_t mesh_factory::ico_vert_count  = 12;
	const size_t mesh_factory::ico_index_count = 60;

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

	static const unsigned int ico_index_data[] = {
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
	//static const unsigned int ico_index_data[] = {
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
	const unsigned int* mesh_factory::ico_index = ico_index_data;

	Mesh* mesh_factory::create_icosphere(
		size_t resolution)
	{
		Mesh* mesh = new Mesh();
		mesh->VertexCount = 12 + (30 * pow(4, resolution));
		mesh->IndexCount  = 60 * pow(4, resolution);
		mesh->Vertices = new iw::vector3[mesh->VertexCount];
		mesh->Normals  = new iw::vector3[mesh->VertexCount];
		mesh->Indices  = new unsigned int[mesh->IndexCount];

		memcpy(mesh->Vertices, ico_verts, ico_vert_count  * sizeof(iw::vector3));
		memcpy(mesh->Indices,  ico_index, ico_index_count * sizeof(unsigned int));

		// Verts & Index

		size_t current_index_count = ico_index_count;
		size_t current_vert_count  = ico_vert_count;
		for (unsigned int i = 0; i < resolution; i++) {
			sub_devide(mesh->Vertices, mesh->Indices, current_index_count, current_vert_count);
		}

		// Normals

		mesh->GenNormals();

		return mesh;
	}

	Mesh* Graphics::mesh_factory::create_uvsphere(
		size_t latCount, 
		size_t lonCount)
	{
		Mesh* mesh = new Mesh();
		mesh->VertexCount = (latCount + 1) * (lonCount + 1);
		mesh->IndexCount  = 6 * (lonCount + (latCount - 2) * lonCount);
		mesh->Vertices = new iw::vector3[mesh->VertexCount];
		mesh->Normals  = new iw::vector3[mesh->VertexCount];
		mesh->Uvs      = new iw::vector2[mesh->VertexCount];
		mesh->Indices  = new unsigned int[mesh->IndexCount];

		// Verts

		float lonStep = 2 * iw::PI / lonCount;
		float latStep  = iw::PI / latCount;

		size_t vert = 0;
		for (size_t lat = 0; lat <= latCount; lat++) {
			for (size_t lon = 0; lon <= lonCount; lon++) {
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
		float v = lonCount + 1;
		for (size_t lon = 0; lon < lonCount; ++lon, v++) {
			mesh->Indices[index++] = lon;
			mesh->Indices[index++] = v;
			mesh->Indices[index++] = v + 1;
		}

		for (size_t lat = 1; lat < latCount - 1; lat++) {
			v = lat * (lonCount + 1);
			for (size_t lon = 0; lon < lonCount; lon++, v++) {
				mesh->Indices[index++] = v;
				mesh->Indices[index++] = v + lonCount + 1;
				mesh->Indices[index++] = v + 1;
				mesh->Indices[index++] = v + 1;
				mesh->Indices[index++] = v + lonCount + 1;
				mesh->Indices[index++] = v + lonCount + 2;
			}
		}

		v = (latCount - 1) * (lonCount + 1);
		for (size_t lon = 0; lon < lonCount; lon++, v++) {
			mesh->Indices[index++] = v;
			mesh->Indices[index++] = v + lonCount + 1;
			mesh->Indices[index++] = v + 1;
		}

		return mesh;
	}

	void mesh_factory::sub_devide(
		iw::vector3* verts,
		unsigned int* index,
		size_t& current_index_count,
		size_t& current_vert_count)
	{
		index_lookup_map lookup;
		unsigned int* next_index = new unsigned int[16U * current_index_count];
		unsigned int next_index_count = 0;

		unsigned int index_count = current_index_count;
		for (unsigned int i = 0; i < index_count; i += 3) {
			unsigned int mid0 = create_vertex_for_edge(lookup, verts, index[i],     index[i + ((i + 1) % 3)], current_vert_count);
			unsigned int mid1 = create_vertex_for_edge(lookup, verts, index[i + 1], index[i + ((i + 2) % 3)], current_vert_count);
			unsigned int mid2 = create_vertex_for_edge(lookup, verts, index[i + 2], index[i + ((i + 3) % 3)], current_vert_count);

			next_index[next_index_count++] = index[i];	   next_index[next_index_count++] = mid0; next_index[next_index_count++] = mid2;
			next_index[next_index_count++] = index[i + 1]; next_index[next_index_count++] = mid1; next_index[next_index_count++] = mid0;
			next_index[next_index_count++] = index[i + 2]; next_index[next_index_count++] = mid2; next_index[next_index_count++] = mid1;
			next_index[next_index_count++] = mid0;	       next_index[next_index_count++] = mid1; next_index[next_index_count++] = mid2;
		}

		memcpy(index, next_index, next_index_count * sizeof(unsigned int));
		current_index_count = next_index_count;

		delete[] next_index;
	}

	unsigned int mesh_factory::create_vertex_for_edge(
		index_lookup_map& lookup,
		iw::vector3* verts,
		unsigned int first,
		unsigned int second,
		size_t& current_vert_count)
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
