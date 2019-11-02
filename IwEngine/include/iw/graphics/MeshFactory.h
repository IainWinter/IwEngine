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

		static const iwm::vector3* ico_verts;
		static const unsigned int* ico_index;
		static const size_t ico_vert_count;
		static const size_t ico_index_count;

	public:
		static Mesh* create_icosphere(
			size_t resolution);

		static Mesh* create_uvsphere(
			size_t latCount,
			size_t lonCount);
	private:
		static void sub_devide(
			iwmath::vector3* verts,
			unsigned int* index,
			size_t& current_index_count,
			size_t& current_vert_count);

		static unsigned int create_vertex_for_edge(
			index_lookup_map& lookup,
			iwm::vector3* verts,
			unsigned int first,
			unsigned int second,
			size_t& current_vert_count);
	};
}
}
