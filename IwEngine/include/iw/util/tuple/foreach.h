#pragma once

#include "iw/util/iwutil.h"
#include <tuple>

namespace iw {
namespace util {
	namespace detail {
#pragma region no_return_no_args
		template<
			typename _functor,
			typename _tuple,
			size_t... _i>
		void foreach_na_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>)
		{
			(functor(std::get<_i>(tuple)), ...);
		}
#pragma endregion

#pragma region no_return_tuple_args
		template<
			typename _functor,
			typename _tuple,
			typename _tuple_args,
			size_t... _i>
		void foreach_ta_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>,
			const _tuple_args& tuple_args)
		{
			(functor(
				std::get<_i>(tuple),
				std::get<_i>(tuple_args)), ...);
		}
#pragma endregion

#pragma region no_return_fixed_args
		template<
			typename _functor,
			typename _tuple,
			typename... _fixed_args,
			size_t... _i>
		void foreach_fa_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>,
			const _fixed_args&... fixed_args)
		{
			(functor(
				std::get<_i>(tuple),
				fixed_args...), ...);
		}
#pragma endregion

#pragma region no_return_tuple_args_and_fixed_args
		template<
			typename _functor,
			typename _tuple,
			typename _tuple_args,
			typename... _fixed_args,
			size_t... _i>
		void foreach_ta_fa_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>,
			const _tuple_args& tuple_arg,
			const _fixed_args&... fixed_args)
		{
			(functor(
				std::get<_i>(tuple),
				std::get<_i>(tuple_arg),
				fixed_args...), ...);
		}
#pragma endregion

#pragma region no_return_tuple_args_and_single_fixed_arg
		template<
			typename _functor,
			typename _tuple,
			typename _fixed_arg,
			typename... _tuple_args,
			size_t... _i>
		void foreach_ta_sfa_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>,
			const _fixed_arg& fixed_arg,
			const _tuple_args&... tuple_args)
		{
			(call_ta_sfa<
				_functor,
				_tuple,
				_i,
				_fixed_arg,
				_tuple_args...>
			(
				std::forward<_functor>(functor),
				tuple,
				fixed_arg,
				tuple_args...
			), ...);
		}

#pragma region callers
		template<
			typename _functor,
			typename _tuple,
			size_t _i,
			typename _fixed_arg,
			typename... _tuple_args>
		void call_ta_sfa(
			_functor&& functor,
			_tuple& tuple,
			const _fixed_arg& fixed_arg,
			const _tuple_args&... tuple_args)
		{
			functor(
				std::get<_i>(tuple),
				fixed_arg,
				std::get<_i>(tuple_args)...
			);
		}
#pragma endregion

#pragma endregion

#pragma region return_no_args
		template<
			typename _functor,
			typename _tuple,
			typename _tuple_return,
			size_t... _i>
		_tuple_return geteach_na_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>)
		{
			return {
				functor(
					std::get<_i>(tuple)
				)...
			};
		}
#pragma endregion

#pragma region return_tuple_args
		template<
			typename _functor,
			typename _tuple,
			typename _tuple_return,
			typename _tuple_args,
			size_t... _i>
		_tuple_return geteach_ta_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>,
			const _tuple_args& tuple_args)
		{
			return {
				functor(
					std::get<_i>(tuple),
					std::get<_i>(tuple_args)
				)...
			};
		}
#pragma endregion

#pragma region return_fixed_args
		template<
			typename _functor,
			typename _tuple,
			typename _tuple_return,
			typename... _fixed_args,
			size_t... _i>
		_tuple_return geteach_fa_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>,
			const _fixed_args&... fixed_args)
		{
			return {
				functor(
					std::get<_i>(tuple),
					fixed_args...
				)...
			};
		}
#pragma endregion

#pragma region return_tuple_args_and_fixed_args
		template<
			typename _functor,
			typename _tuple,
			typename _tuple_return,
			typename _tuple_args,
			typename... _fixed_args,
			size_t... _i>
		_tuple_return geteach_ta_fa_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>,
			const _tuple_args& tuple_args,
			const _fixed_args&... fixed_args)
		{
			return {
				functor(
					std::get<_i>(tuple),
					std::get<_i>(tuple_args),
					fixed_args...
				)...
			};
		}
#pragma endregion

#pragma region return_tuple_args_and_single_fixed_arg
		template<
			typename _functor,
			typename _tuple,
			typename _tuple_return,
			typename _fixed_arg,
			typename... _tuple_args,
			size_t... _i>
		void geteach_ta_sfa_indexed(
			_functor&& functor,
			_tuple& tuple,
			std::index_sequence<_i...>,
			const _fixed_arg& fixed_arg,
			const _tuple_args&... tuple_args)
		{
			return {
				call_ta_sfa<_i>(
					std::forward<_functor>(functor),
					tuple,
					fixed_arg,
					tuple_args...
				)...
			};
		}

#pragma region callers
		template<
			typename _functor,
			typename _tuple,
			typename _tuple_return,
			size_t _i,
			typename _fixed_arg,
			typename... _tuple_args>
		_tuple_return get_ta_sfa(
			_functor&& functor,
			_tuple& tuple,
			const _fixed_arg& fixed_arg,
			const _tuple_args&... tuple_args)
		{
			return functor(
				std::get<_i>(tuple),
				fixed_arg,
				std::get<_i>(tuple_args)...
			);
		}
#pragma endregion

#pragma endregion
	}

#pragma region no_return_no_args
	template<
		typename _functor,
		size_t _size,
		typename _tuple>
	void foreach(
		_tuple& tuple)
	{
		detail::foreach_na_indexed<
			_functor,
			_tuple>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{}
		);
	}
#pragma endregion

#pragma region no_return_tuple_args
	template<
		typename _functor,
		size_t _size,
		typename _tuple,
		typename _tuple_args>
	void foreach(
		_tuple& tuple,
		const _tuple_args& tuple_args)
	{
		detail::foreach_ta_indexed<
			_functor,
			_tuple,
			_tuple_args>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{},
			tuple_args
		);
	}
#pragma endregion

#pragma region no_return_fixed_args
	template<
		typename _functor,
		size_t _size,
		typename _tuple,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		const _fixed_args&... fixed_args)
	{
		detail::foreach_fa_indexed<
			_functor,
			_tuple,
			_fixed_args...>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{},
			fixed_args...
		);
	}
#pragma endregion

#pragma region no_return_tuple_args_and_fixed_args
	template<
		typename _functor,
		size_t _size,
		typename _tuple,
		typename _tuple_args,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		const _tuple_args& tuple_args,
		const _fixed_args&... fixed_args)
	{
		detail::foreach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{},
			tuple_args,
			fixed_args...
		);
	}
#pragma endregion

#pragma region no_return_tuple_args_and_single_fixed_arg
	template<
		typename _functor,
		size_t _size,
		typename _tuple,
		typename _fixed_arg,
		typename... _tuple_args>
	void formatrix(
		_tuple& tuple,
		const _fixed_arg& fixed_args,
		const _tuple_args&... tuple_args)
	{
		detail::foreach_ta_sfa_indexed<
			_functor,
			_tuple,
			_fixed_arg,
			_tuple_args...>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{},
			fixed_args,
			tuple_args...
		);
	}
#pragma endregion

#pragma region return_no_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		size_t _size>
	_tuple_return geteach(
		_tuple& tuple)
	{
		return detail::geteach_na_indexed<
			_functor,
			_tuple,
			_tuple_return>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{}
		);
	}
#pragma endregion

#pragma region return_tuple_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		size_t _size>
	_tuple_return geteach(
		_tuple& tuple,
		const _tuple_args& tuple_args)
	{
		return detail::geteach_ta_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{},
			tuple_args
		);
	}
#pragma endregion

#pragma region return_fixed_args
	template<
		typename _functor,
		size_t _size,
		typename _tuple,
		typename _tuple_return,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		const _fixed_args&... fixed_args)
	{
		return detail::geteach_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_fixed_args...>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{},
			fixed_args...
		);
	}
#pragma endregion

#pragma region return_tuple_args_and_fixed_args
	template<
		typename _functor,
		size_t _size,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		const _tuple_args& tuple_args,
		const _fixed_args&... fixed_args)
	{
		return detail::geteach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{},
			tuple_args,
			fixed_args...
		);
	}
#pragma endregion

#pragma region no_return_tuple_args_and_single_fixed_arg
	template<
		typename _functor,
		size_t _size,
		typename _tuple,
		typename _tuple_return,
		typename _fixed_arg,
		typename... _tuple_args>
	_tuple_return getmatrix(
		_tuple& tuple,
		const _fixed_arg& fixed_args,
		const _tuple_args&... tuple_args)
	{
		return detail::geteach_ta_sfa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_fixed_arg,
			_tuple_args...>
		(
			_functor(),
			tuple,
			std::make_index_sequence<_size>{},
			fixed_args,
			tuple_args...
		);
	}
#pragma endregion
}

namespace functors {
	//Calls operator++()
	struct increment {
		template<
			typename _t>
		void operator()(
			_t& t)
		{
			t++;
		}
	};

	//Calls operator--()
	struct decrement {
		template<
			typename _t>
		void operator()(
			_t& t)
		{
			t--;
		}
	};

	//Calls delete
	struct erase {
		template<typename _t>
		void operator()(
			_t& t)
		{
			delete t;
			t = nullptr;
		}
	};

	//Calls delete[]
	struct erase_array {
		template<
			typename _t>
		void operator()(
			_t& t)
		{
			delete[] t;
			t = nullptr;
		}
	};

	//Calls operator=(const _t&)
	struct assign {
		template<
			typename _t>
		void operator()(
			_t& t,
			const _t& d)
		{
			t = d;
		}
	};

	//Calls operator*()
	struct reference {
		template<
			typename _t>
		typename _t::value_type& operator()(
			_t& t)
		{
			return *std::forward<_t>(t);
		}
	};

	struct anyequals {
		template<
			typename _t,
			typename _o>
		void operator()(
			_t& t,
			_o& o,
			bool& equals)
		{
			equals = t == o;
		}
	};
}

	using namespace util;
}
