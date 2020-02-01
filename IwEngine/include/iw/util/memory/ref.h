#pragma once

#include "iw/util/iwutil.h"
#include <memory>

namespace iw {
namespace util {
	template<
		typename _t>
	using unique = std::unique_ptr<_t>;

	template<
		typename _t>
	using ref = std::shared_ptr<_t>;

	template<
		typename _t>
	using weak = std::weak_ptr<_t>;

	#define REF std::make_shared
}

	using namespace util;
}
