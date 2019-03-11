#pragma once

#include "iw/events/iwevents.h"
#include "iw/events/functional/signal.h"

namespace iwevents {
namespace functional {
	template<
		typename... _ts>
	class eventbus
		: public signal<_ts>...
	{
	public:
		template<
			typename _t,
			typename _c>
		void subscribe(
			_c* instance,
			void(_c::* function)(_t))
		{
			signal<_t>::subscribe(instance, function);
		}

		template<
			typename _t>
		void emit(
			_t arg)
		{
			signal<_t>::emit(arg);
		}
	};
}
}
