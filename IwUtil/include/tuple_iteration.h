#pragma once

#include <tuple>

namespace iwutil {
	// n arg with n element
	template<
		typename _functor,
		typename _tuple,
		typename _arg_tuple,
		std::size_t... _index>
	void foreach(
		_functor&& functor,
		_tuple& tuple,
		_arg_tuple& args,
		std::index_sequence<_index...>)
	{
		auto e = {(
			functor(std::get<_index>(tuple), std::get<_index>(args)),
			0)...
		};
	}

	// n arg with n element
	template<
		typename _functor,
		typename _tuple,
		typename _arg_tuple,
		std::size_t _size>
	void foreach(
		_tuple& tuple,
		_arg_tuple& arg_tuple)
	{
		foreach(_functor(), tuple, arg_tuple,
			std::make_index_sequence<_size>{});
	}

	// n arg with n element
	template<
		typename _functor,
		typename _tuple,
		typename _arg_tuple,
		std::size_t... _index>
	void foreach(
		_functor&& functor,
		_tuple& tuple,
		const _arg_tuple& args,
		std::index_sequence<_index...>)
	{
		auto e = {(
			functor(std::get<_index>(tuple), std::get<_index>(args)),
			0)...
		};
	}

	// n arg with n element
	template<
		typename _functor,
		typename _tuple,
		typename _arg_tuple,
		std::size_t _size>
	void foreach(
		_tuple& tuple,
		const _arg_tuple& arg_tuple)
	{
		foreach(_functor(), tuple, arg_tuple,
			std::make_index_sequence<_size>{});
	}

	// all args with n element
	template<
		typename _functor,
		typename _tuple,
		typename... _args,
		std::size_t... _index>
	void foreach(
		_functor&& functor,
		_tuple & tuple,
		_args&&... args,
		std::index_sequence<_index...>)
	{
		auto e = {(
			functor(std::get<_index>(tuple), std::forward<_args>(args)...),
			0)...
		};
	}

	// all args with n element
	template<
		typename _functor,
		typename _tuple,
		typename... _args,
		std::size_t... _index>
	void foreach(
		_functor&& functor,
		_tuple & tuple,
		const _args&... args,
		std::index_sequence<_index...>)
	{
		auto e = {(
			functor(std::get<_index>(tuple), args...),
			0)...
		};
	}

	// all args with n element
	template<
		typename _functor,
		typename _tuple,
		std::size_t _size,
		typename... _args>
	void foreach(
		_tuple& tuple,
		_args&&... args)
	{
		foreach(_functor(), tuple, std::forward<_args>(args)...,
			std::make_index_sequence<_size>{});
	}

	// all args with n element
	template<typename _functor,
		typename _tuple,
		std::size_t _size,
		typename... _args>
	void foreach(
		_tuple& tuple, 
		_args&... args)
	{
		foreach(_functor(), tuple, std::forward<_args>(args)...,
			std::make_index_sequence<_size>{});
	}

	// all args with n element
	template<typename _functor,
		typename _tuple,
		std::size_t _size,
		typename... _args>
	void foreach(
		_tuple& tuple, 
		const _args&... args)
	{
		foreach(_functor(), tuple, args...,
			std::make_index_sequence<_size>{});
	}

	// no args
	template<
		typename _functor,
		typename _tuple,
		std::size_t... _index>
	void foreach(
		_functor&& functor,
		_tuple& tuple,
		std::index_sequence<_index...>)
	{
		auto e = {(
			functor(std::get<_index>(tuple)),
			0)...
		};
	}

	// no args
	template<
		typename _functor,
		typename _tuple,
		std::size_t _size>
	void foreach(
		_tuple& tuple)
	{
		foreach(_functor(), tuple, std::make_index_sequence<_size>{});
	}


// return

// no args
	template<
		typename _functor,
		typename _tuple,
		typename _return_tuple,
		std::size_t... _index>
	_return_tuple geteach(
		_functor&& functor,
		_tuple& tuple,
		std::index_sequence<_index...>)
	{
		return _return_tuple(functor(std::get<_index>(tuple))...);
	}

	// no args
	template<
		typename _functor,
		typename _tuple,
		typename _return_tuple,
		std::size_t _size>
	_return_tuple geteach(
		_tuple& tuple)
	{
		return geteach<_functor, _tuple, _return_tuple>(_functor(), tuple,
			std::make_index_sequence<_size>{});
	}
}

namespace functors {
	struct increment {
		template<
			typename _t>
			void operator()(_t&& t) {
			t++;
		}
	};

	struct decrement {
		template<
			typename _t>
			void operator()(_t&& t) {
			t--;
		}
	};

	struct erase {
		template<
			typename _t>
			void operator()(_t&& t) {
			delete t;
		}
	};

	struct erase_array {
		template<
			typename _t>
			void operator()(_t&& t) {
			delete[] t;
		}
	};

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

	struct reference {
		template<
			typename _t>
		typename std::remove_pointer<typename std::remove_reference<_t>::type>::type& operator()(
			_t&& t)
		{
			return *std::forward<_t>(t);
		}
	};
}