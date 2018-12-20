#pragma once

#include "tight_vector.h"

namespace iwutil {
	template<
		typename _t>
	class sparse_set {
	public:
	private:
		iwutil::tight_vector<std::size_t> m_index;
		iwutil::tight_vector<_t> m_set;

	public:
	private:
	};
}