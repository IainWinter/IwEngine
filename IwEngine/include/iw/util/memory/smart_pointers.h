#pragma once

#include <memory>

namespace std {
	template<
		typename _t>
	using uptr = std::unique_ptr<_t>;

	template<
		typename _t>
	using sptr = std::shared_ptr<_t>;

	template<
		typename _t>
	using wptr = std::weak_ptr<_t>;
}
