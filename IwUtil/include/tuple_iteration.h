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
		};
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
				std::get<_index>(std::forward(tuple_args))),
			0)...
		};
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
		};
	}

	//Tuple args &&
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
		auto e = {(
			functor(
				std::get<_index>(std::forward(tuple)),
				std::get<_index>(tuple_args),
				std::forward<_fixed_args>(fixed_args)...),
			0)...
		};
	}

	//Tuple args const &
	//Args const &
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
		const _tuple_args& tuple_args,
		const _fixed_args&... fixed_args,
		std::index_sequence<_index...>)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward(tuple)),
				tuple_args,
				fixed_args...),
			0)...
		};
	}

	//No args
	//Return
	template<
		typename _functor,
		typename _tuple,
		typename _return_tuple,
		std::size_t... _index>
	_return_tuple foreach(
		_functor&& functor,
		_tuple&& tuple,
		std::index_sequence<_index...>)
	{
		return _return_tuple(
			functor(
				std::get<_index>(std::forward(tuple)))...);
	}

	//Tuple args &&
	//Return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t... _index>
	_tuple_return foreach(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		std::index_sequence<_index...>)
	{
		return _tuple_return(
			functor(
				std::get<_index>(std::forward(tuple)),
				std::get<_index>(std::forward(tuple_args)))...);
	}

	//Tuple args const &
	//Return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t... _index>
	_tuple_return foreach(
		_functor&& functor,
		_tuple&& tuple,
		const _tuple_args& tuple_args,
		std::index_sequence<_index...>)
	{
		return _tuple_return(
			functor(
				std::get<_index>(std::forward(tuple)),
				std::get<_index>(tuple_args))...);
	}

	//Tuple args &&
	//Args &&
	//Return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
	_tuple_return foreach(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		_fixed_args&&... fixed_args,
		std::index_sequence<_index...>)
	{
		return _tuple_return(
			functor(
				std::get<_index>(std::forward(tuple)),
				std::get<_index>(tuple_args),
				std::forward<_fixed_args>(fixed_args)...)...);
	}

	//Tuple args const &
	//Args const &
	//Return
	template<
		typename _functor,
		typename _tuple,
		typename _return_tuple,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
	_return_tuple foreach(
		_functor&& functor,
		_tuple&& tuple,
		const _tuple_args& tuple_args,
		const _fixed_args&... fixed_args,
		std::index_sequence<_index...>)
	{
		return _return_tuple(
			functor(
				std::get<_index>(std::forward(tuple)),
				tuple_args,
				fixed_args...)...);
	}

	//Helper functions

	//No args
	//No return
	template<
		typename _functor,
		typename _tuple,
		std::size_t _size>
	void foreach(
		_functor&& functor,
		_tuple&& tuple)
	{
		foreach<_functor, _tuple>(
			std::forward(functor),
			std::forward(tuple),
			std::integer_sequence<_size>{});
	}

	//Tuple args &&
	//No return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _size>
	void foreach(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args)
	{
		foreach<_functor, _tuple, _tuple_args>(
			std::forward(functor),
			std::forward(tuple),
			std::forward(tuple_args),
			std::index_sequence<_size>{});
	}

	//Tuple args &
	//No return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _size>
	void foreach(
		_functor&& functor,
		_tuple& tuple,
		_tuple_args& tuple_args)
	{
		foreach<_functor, _tuple, _tuple_args>(
			std::forward(functor),
			std::forward(tuple),
			std::forward(tuple_args),
			std::index_sequence<_size>{});
	}

	//Tuple args const &
	//No return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _size>
	void foreach(
		_functor&& functor,
		_tuple&& tuple,
		const _tuple_args& tuple_args)
	{
		foreach<_functor, _tuple, _tuple_args>(
			std::forward(functor),
			std::forward(tuple),
			tuple_args,
			std::index_sequence<_size>{});
	}

	//Tuple args &&
	//Args &&
	//No return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _size,
		typename... _fixed_args>
	void foreach(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		_fixed_args&&... fixed_args)
	{
		foreach<_functor, _tuple, _tuple_args, _fixed_args...>(
			std::forward(functor),
			std::forward(tuple),
			std::forward(tuple_args),
			std::forward(fixed_args)...,
			std::index_sequence<_size>{});
	}

	//Tuple args &
	//Args &
	//No return
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _size,
		typename... _fixed_args>
	void foreach(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args& tuple_args,
		_fixed_args&... fixed_args)
	{
		foreach<_functor, _tuple, _tuple_args, _fixed_args...>(
			std::forward(functor),
			std::forward(tuple),
			std::forward(tuple_args),
			std::forward(fixed_args)...,
			std::index_sequence<_size>{});
	}

	//Tuple args const &
	//Args const &
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
		const _tuple_args& tuple_args,
		const _fixed_args&... fixed_args,
		std::index_sequence<_index...>)
	{
		foreach<_functor, _tuple, _tuple_args, _fixed_args...>(
			std::forward(functor),
			std::forward(tuple),
			tuple_args,
			fixed_args...,
			std::index_sequence<_size>{});
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