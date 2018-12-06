#pragma once

#include <tuple>
#include "ct_iteration.h"
#include "archtype.h"

namespace iwutil {
	template<typename... _t>
	class tuple : public std::tuple<_t...> {
	public:
		using archtype_t = archtype<_t...>;
	private:
		using tuple_t    = std::tuple<_t...>;

	public:
		tuple()
			: std::tuple<_t...>() {}

		tuple(_t&&... args)
			: std::tuple<_t...>(std::forward<_t>(args)...) {}

		tuple(_t&... args)
			: std::tuple<_t...>(std::forward<_t>(args)...) {}

		tuple(tuple&& copy)
			: std::tuple<_t...>(std::forward<tuple>(copy)) {}

		tuple(const tuple& copy)
			: std::tuple<_t...>(copy) {}

		template<typename _functor_t>
		void foreach(_functor_t&& functor) {
			iwutil::foreach<_functor_t, tuple_t, archtype_t::size>(*this);
		}
	};
}