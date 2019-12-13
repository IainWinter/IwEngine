#pragma once

namespace iw {
namespace events {
	struct event {
		short Group;
		short Type;
		bool Handled;

		event()
			: Group(0)
			, Type(0)
			, Handled(false)
		{}

		event(
			short category,
			short type)
			: Group(category)
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

	using namespace events;
}
