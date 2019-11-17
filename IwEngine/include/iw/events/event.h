#pragma once

namespace iw {
inline namespace events {
	struct event {
		short Category;
		short Type;
		bool Handled;

		event()
			: Category(0)
			, Type(0)
			, Handled(false)
		{}

		event(
			short category,
			short type)
			: Category(category)
			, Type(type)
			, Handled(false)
		{}

		template<
			typename _t>
		_t& as() {
			return *(_t*)this;
		}
	};
}
}
