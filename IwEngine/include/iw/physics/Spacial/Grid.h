#pragma once

#include "iw/physics/impl/grid.h"

namespace iw {
namespace Physics {
	template<typename _t> using grid  = impl::grid<_t, d3>;
	template<typename _t> using grid2 = impl::grid<_t, d2>;
}

	using namespace Physics;
}
