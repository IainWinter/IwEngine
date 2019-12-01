#pragma once
#include "Mesh.h"
#include "iw/util/algorithm/pair_hash.h"
#include <unordered_map>

namespace IW {
inline namespace Graphics {
	// why is this snek
	class IWGRAPHICS_API mesh_factory {
	private:
		typedef std::pair<unsigned, unsigned> index_pair;
		typedef std::unordered_map<index_pair, unsigned, iw::pair_hash> index_lookup_map;
		typedef std::vector<iw::vector3> vert_list;

		static const unsigned ico_vert_count;
		static const unsigned ico_index_count;
		static const unsigned*    ico_index;
		static const iw::vector3* ico_verts;

	public:
		static Mesh* create_icosphere(
			unsigned resolution);

		static Mesh* create_uvsphere(
			unsigned latCount,
			unsigned lonCount);
	private:
		static void sub_devide(
			iw::vector3* verts,
			unsigned* index,
			unsigned& current_index_count,
			unsigned& current_vert_count);

		static unsigned int create_vertex_for_edge(
			index_lookup_map& lookup,
			iw::vector3* verts,
			unsigned first,
			unsigned second,
			unsigned& current_vert_count);
	};
}
}
