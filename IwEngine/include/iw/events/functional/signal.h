#pragma once

#include "callback.h"
#include <functional>
#include <vector>

namespace iwevents {
namespace functional {
	template<
		typename... _args_t>
	class signal {
	private:
		std::vector<callback<_args_t...>> m_callbacks;

	public:
		template<
			typename _t>
		void subscribe(
			_t* instance,
			void(_t::*function)(_args_t...))
		{
			m_callbacks.emplace_back(make_callback(function, instance));
		}

		void emit(
			_args_t... arg)
		{
			for (auto& callback : m_callbacks) {
				callback(arg...);
			}
		}
	};
}
}
