#pragma once

namespace iw {
namespace events {
	struct event {
		short Group;
		short Type;
		bool Handled;
		int Size; // might get removed

		event()
			: Group(0)
			, Type(0)
			, Handled(false)
			, Size(0)
		{}

		event(
			short category,
			short type)
			: Group(category)
			, Type(type)
			, Handled(false)
			, Size(0)
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
