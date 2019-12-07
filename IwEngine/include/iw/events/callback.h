#pragma once

#include "binding.h"

namespace iw {
namespace events {
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

		void operator()(
			_args_t... arg)
		{
			if (m_callback) {
				m_callback(arg...);
			}
		}
	};

	template<
		typename _r,
		typename... _args_t>
	class getback {
	public:
		using function_type = _r(_args_t...);
	private:
		std::function<function_type> m_getback;

	public:
		getback() = default;

		getback(
			function_type getback)
			: m_getback(getback)
		{}

		getback(
			std::function<function_type> getback)
			: m_getback(getback)
		{}

		_r operator()(
			_args_t... arg)
		{
			if (m_getback) {
				return m_getback(arg...);
			}
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
		void(_t::* function)(_args_t...),
		_t* instance)
	{
		return callback<_args_t...>(
			bind<void, _t*, _args_t...>(function, instance));
	}

	template<
		typename _r,
		typename... _args_t>
	getback<_r, _args_t...> make_getback(
		_r(function)(_args_t...))
	{
		return getback<_r, _args_t...>(function);
	}

	template<
		typename _t,
		typename _r,
		typename... _args_t>
	getback<_r, _args_t...> make_getback(
		_r(_t::* function)(_args_t...),
		_t* instance)
	{
		return getback<_r, _args_t...>(
			bind<_r, _t*, _args_t...>(function, instance));
	}
}

	using namespace events;
}
