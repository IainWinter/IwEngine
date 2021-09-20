#pragma once

#include "iw/util/reflection/ReflectDef.h"

namespace iw {
namespace events {
	struct REFLECT event {
		short Group;
		short Type;
		bool Handled;
		int Size; // might get removed

		size_t Id;

		event()
			: Group(0)
			, Type(0)
			, Handled(false)
			, Size(0)
			, Id(0)
		{}

		event(
			short category,
			short type
		)
			: Group(category)
			, Type(type)
			, Handled(false)
			, Size(0)
			, Id(0)
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
