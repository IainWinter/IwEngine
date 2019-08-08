#pragma once

#include "iw/util/iwutil.h"

namespace iwutil {
	template<
		typename...>
	class family {
	private:
		inline static size_t id;

		template<
			typename...>
		inline static const size_t inner_id = id++;

	public:
		template<
			typename... _t>
		inline static const size_t type = inner_id<std::decay_t<_t>...>;
	};
}
