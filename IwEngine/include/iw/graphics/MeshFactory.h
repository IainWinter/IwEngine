#pragma once
#include "Mesh.h"
#include "iw/util/algorithm/pair_hash.h"
#include <unordered_map>

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API mesh_factory {
	private:
		typedef std::pair<unsigned int, unsigned int> index_pair;
		typedef std::unordered_map<index_pair, unsigned int, iwu::pair_hash> index_lookup_map;
		typedef std::vector<iwm::vector3> vert_list;

		static const unsigned int ico_vert_count;
		static const unsigned int ico_index_count;
		static const iwm::vector3* ico_verts;
		static const unsigned int* ico_index;

		static void sub_devide(
			iwmath::vector3* verts,
			unsigned int* index,
			unsigned int& current_index_count,
			unsigned int& current_vert_count);

		static unsigned int create_vertex_for_edge(
			index_lookup_map& lookup,
			iwm::vector3* verts,
			unsigned int first,
			unsigned int second,
			unsigned int& current_vert_count);
	public:
		static Mesh* create_icosphere(const unsigned int& resolution);
	};
}
}
