#pragma once

#include <tuple>

namespace iwutil {
	//no args with n element
	//n arg with n element
	//all args with n element
	//n arg and all fixed args with n element

	//return from no args with n element
	//return from all args with n element
	//return from n arg and all fixed args with n element

	//No args
	//No return
	template<
		typename _functor,
		typename _tuple,
		std::size_t... _index>
	void foreach(
		_functor&& functor,
		_tuple&& tuple,
		std::index_sequence<_index...>)
	{
		auto e = {(
			functor(std::get<_index>(std::forward(tuple))),
			0)...
		}
	}

	//Tuple args &&
	//No return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t... _index>
	void foreach(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		std::index_sequence<_index...>)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward(tuple)), 
				std::get<_index>(std::forward(tuple_args)),
			0)...
		}
	}

	//Tuple args const &
	//No return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t... _index>
		void foreach(
			_functor&& functor,
			_tuple&& tuple,
			const _tuple_args& tuple_args,
			std::index_sequence<_index...>)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward(tuple)),
				std::get<_index>(tuple_args)),
			0)...
		}
	}

	//Tuple args &
	//Args &&
	//No return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
		void foreach(
			_functor&& functor,
			_tuple&& tuple,
			_tuple_args&& tuple_args,
			_fixed_args&&... fixed_args,
			std::index_sequence<_index...>)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward(tuple)),
				std::get<_index>(tuple_args)),
				std::forward<_fixed_args>(fixed_args)...
			0)...
		}
	}

	//Tuple args &
	//Args &
	//No return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
		void foreach(
			_functor&& functor,
			_tuple&& tuple,
			_tuple_args&& tuple_args,
			_fixed_args&&... fixed_args,
			std::index_sequence<_index...>)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward(tuple)),
				std::get<_index>(tuple_args)),
				std::forward<_fixed_args>(fixed_args)...
			0)...
		}
	}



	//////////

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

	// n arg and all fixed args with n element
	template<
		typename _functor,
		typename _tuple,
		typename _arg_tuple,
		typename... _fixed_args,
		std::size_t... _index>
	void foreach(
		_functor&& functor,
		_tuple& tuple,
		const _arg_tuple& args,
		const _fixed_args&... fixed_args,
		std::index_sequence<_index...>)
	{
		auto e = { (
			functor(std::get<_index>(tuple), std::get<_index>(args), fixed_args...),
			0)...
		};
	}

	// n arg and all fixed args with n element
	template<
		typename _functor,
		typename _tuple,
		typename _arg_tuple,
		std::size_t _size,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		const _arg_tuple& arg_tuple,
		const _fixed_args&... fixed_args)
	{
		foreach(_functor(), tuple, arg_tuple, fixed_args...,
			std::make_index_sequence<_size>{});
	}


// returning foreach //

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
		void operator()(
			_t&& t)
		{
			t++;
		}
	};

	struct decrement {
		template<
			typename _t>
		void operator()(_t&& t)
		{
			t--;
		}
	};

	struct erase {
		template<typename _t>
		void operator()(
			_t&& t)
		{
			delete t;
		}
	};

	struct erase_array {
		template<
			typename _t>
		void operator()(
			_t&& t)
		{
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
		typename std::remove_pointer<typename std::remove_reference<_t>
			::type>::type& operator()(
			_t&& t)
		{
			return *std::forward<_t>(t);
		}
	};
}