#pragma once

#include "binding.h"
#include <functional>

namespace iwevents {
	template<
		typename... _args_t>
	class callback {
	public:
		using function_type = void(_args_t...);
	private:
		std::function<function_type> m_callback;

	public:
		callback() = default;

		callback(
			function_type callback)
			: m_callback(callback)
		{}

		callback(
			std::function<function_type> callback)
			: m_callback(callback)
		{}

		void operator()(_args_t... arg) {
			m_callback(arg...);
		}
	};

	template<
		typename... _args_t>
	callback<_args_t...> make_callback(
		void(function)(_args_t...))
	{
		return callback<_args_t...>(function);
	}

	template<
		typename _t,
		typename... _args_t>
	callback<_args_t...> make_callback(
		void(_t::*function)(_args_t...),
		_t* instance)
	{
		return callback<_args_t...>(
			bind<void, _t*, _args_t...>(function, instance));
	}
}
