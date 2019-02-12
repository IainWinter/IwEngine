#pragma once

#include "component/archetype.h"

namespace iwent {
	class sub_space {
	private:
		archetype m_archetype;
		//list of space_sets of components
		//
	public:
		sub_space(
			archetype archetype)
			: m_archetype(archetype)
		{}

		bool is_similar(archetype a) {
			return has_any(m_archetype, a);
		}
	};
}
