#include "iw/graphics/MeshFactory.h"
#define ICO_X .525731112119133606f
#define ICO_Z .850650808352039932f

namespace IW {
	const unsigned int mesh_factory::ico_vert_count = 12;
	const unsigned int mesh_factory::ico_index_count = 60;

	static const iwmath::vector3 ico_vert_data[] = {
		iwmath::vector3(-ICO_X,  0,	    ICO_Z),
		iwmath::vector3(ICO_X,  0,	    ICO_Z),
		iwmath::vector3(-ICO_X,  0,     -ICO_Z),
		iwmath::vector3(ICO_X,  0,     -ICO_Z),
		iwmath::vector3(0,		 ICO_Z,  ICO_X),
		iwmath::vector3(0,		 ICO_Z, -ICO_X),
		iwmath::vector3(0,     -ICO_Z,  ICO_X),
		iwmath::vector3(0,	    -ICO_Z, -ICO_X),
		iwmath::vector3(ICO_Z,  ICO_X,  0),
		iwmath::vector3(-ICO_Z,  ICO_X,  0),
		iwmath::vector3(ICO_Z, -ICO_X,  0),
		iwmath::vector3(-ICO_Z, -ICO_X,  0)
	};

	static const unsigned int ico_index_data[] = {
		0,  4,  1,
		0,  9,  4,
		9,  5,  4,
		4,  5,  8,
		4,  8,  1,
		8, 10,  1,
		8,  3, 10,
		5,  3,  8,
		5,  2,  3,
		2,  7,  3,
		7, 10,  3,
		7,  6, 10,
		7, 11,  6,
		11,  0,  6,
		0,  1,  6,
		6,  1, 10,
		9,  0, 11,
		9, 11,  2,
		9,  2,  5,
		7,  2, 11
	};

	const iwmath::vector3* mesh_factory::ico_verts = ico_vert_data;
	const unsigned int* mesh_factory::ico_index = ico_index_data;

	Mesh* mesh_factory::create_icosphere(const unsigned int& resolution) {
		unsigned int vert_count = 12 + (30 * pow(4, resolution));
		iwmath::vector3* verts = new iwmath::vector3[vert_count];
		memcpy(verts, ico_verts, ico_vert_count * sizeof(iwmath::vector3));

		unsigned int index_count = 60 * pow(4, resolution);
		unsigned int* index = new unsigned int[index_count];
		memcpy(index, ico_index, ico_index_count * sizeof(unsigned int));

		unsigned int current_index_count = ico_index_count;
		unsigned int current_vert_count = ico_vert_count;
		for (unsigned int i = 0; i < resolution; i++) {
			sub_devide(verts, index, current_index_count, current_vert_count);
		}

		// Calculate normals

		unsigned int norm_count = 12 + (30 * pow(4, resolution));
		iwmath::vector3* norms = new iwmath::vector3[norm_count];
		memcpy(verts, ico_verts, ico_vert_count * sizeof(iwmath::vector3));

		for (size_t i = 0; i < index_count; i += 3) {
			iwm::vector3 v1 = verts[index[i + 0]];
			iwm::vector3 v2 = verts[index[i + 1]];
			iwm::vector3 v3 = verts[index[i + 2]];

			iwm::vector3 face = (v3 - v1).cross(v2 - v1).normalized();

			norms[index[i + 0]] = (norms[index[i + 0]] + face).normalized();
			norms[index[i + 1]] = (norms[index[i + 1]] + face).normalized();
			norms[index[i + 2]] = (norms[index[i + 2]] + face).normalized();
		}

		Mesh* mesh = new Mesh();
		mesh->SetVertices(vert_count, verts);
		mesh->SetNormals (norm_count, norms);
		mesh->SetIndices (index_count, index);

		return mesh;
	}

	void mesh_factory::sub_devide(
		iwmath::vector3* verts,
		unsigned int* index,
		unsigned int& current_index_count,
		unsigned int& current_vert_count)
	{
		index_lookup_map lookup;
		unsigned int* next_index = new unsigned int[16 * current_index_count];
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
		iwmath::vector3* verts,
		unsigned int first,
		unsigned int second,
		unsigned int& current_vert_count)
	{
		index_pair key = first > second ? index_pair(first, second) : index_pair(second, first);

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
