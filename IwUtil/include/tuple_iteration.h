#pragma once

#include <tuple>

namespace iwutil {

#pragma region Iteration

#pragma region no_return_no_args
	template<
		typename _functor,
		typename _tuple,
		std::size_t... _index>
	void foreach_na_indexed(
		_functor&& functor,
		_tuple&& tuple,
		std::index_sequence<_index...>)
	{
		auto e = {(
			functor(std::get<_index>(std::forward<_tuple>(tuple))),
			0)...
		};
	}
#pragma endregion

#pragma region no_tuple_return_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t... _index>
	void foreach_ta_indexed(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		std::index_sequence<_index...>)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)), 
				std::get<_index>(std::forward<_tuple_args>(tuple_args))),
			0)...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t... _index>
	void foreach_ta_indexed(
		_functor&& functor,
		_tuple&& tuple,
		const _tuple_args& tuple_args,
		std::index_sequence<_index...>)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::get<_index>(tuple_args)),
			0)...
		};
	}
#pragma endregion

#pragma region no_return_fixed_args
	template<
		typename _functor,
		typename _tuple,
		typename... _fixed_args,
		std::size_t... _index>
	void foreach_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		std::index_sequence<_index...>,
		_fixed_args&&... fixed_args)
	{
		auto e = {(
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::forward<_fixed_args>(fixed_args)...),
			0)...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename... _fixed_args,
		std::size_t... _index>
	void foreach_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		std::index_sequence<_index...>,
		const _fixed_args&... fixed_args)
	{
		auto e = {(
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				fixed_args...),
			0)...
		};
	}
#pragma endregion

#pragma region no_tuple_return_args_and_fixed_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
	void foreach_ta_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		std::index_sequence<_index...>,
		_fixed_args&&... fixed_args)
	{
		auto e = {(
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::get<_index>(tuple_args),
				std::forward<_fixed_args>(fixed_args)...),
			0)...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
	void foreach_ta_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		const _tuple_args& tuple_args,
		std::index_sequence<_index...>,
		const _fixed_args&... fixed_args)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::get<_index>(tuple_args),
				fixed_args...),
			0)...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
	void foreach_ta_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		const _tuple_args& tuple_args,
		std::index_sequence<_index...>,
		_fixed_args&&... fixed_args)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::get<_index>(tuple_args),
				std::forward<_fixed_args>(fixed_args)...),
			0)...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
	void foreach_ta_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		std::index_sequence<_index...>,
		const _fixed_args&... fixed_args)
	{
		auto e = { (
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::forward<_tuple_args>(tuple_args),
				fixed_args...),
			0)...
		};
	}
#pragma endregion

#pragma region return_no_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		std::size_t... _index>
	_tuple_return geteach_na_indexed(
		_functor&& functor,
		_tuple&& tuple,
		std::index_sequence<_index...>)
	{
		return {
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)))...
		};
	}
#pragma endregion

#pragma region return_tuple_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t... _index>
	_tuple_return geteach_ta_indexed(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		std::index_sequence<_index...>)
	{
		return {
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::get<_index>(std::forward(tuple_args)))...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t... _index>
	_tuple_return geteach_ta_indexed(
		_functor&& functor,
		_tuple&& tuple,
		const _tuple_args& tuple_args,
		std::index_sequence<_index...>)
	{
		return {
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::get<_index>(tuple_args))...
		};
	}
#pragma endregion

#pragma region return_fixed_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename... _fixed_args,
		std::size_t... _index>
	_tuple_return geteach_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		std::index_sequence<_index...>,
		_fixed_args&&... fixed_args)
	{
		return {
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::forward<_fixed_args>(fixed_args)...)...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename... _fixed_args,
		std::size_t... _index>
	_tuple_return geteach_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		std::index_sequence<_index...>,
		const _fixed_args&... fixed_args)
	{
		return {
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				fixed_args...)...
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
		std::size_t... _index>
	_tuple_return geteach_ta_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		std::index_sequence<_index...>,
		_fixed_args&&... fixed_args)
	{
		return {
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::get<_index>(tuple_args),
				std::forward<_fixed_args>(fixed_args)...)...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
	_tuple_return geteach_ta_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		const _tuple_args& tuple_args,
		std::index_sequence<_index...>,
		const _fixed_args&... fixed_args)
	{
		return {
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				tuple_args,
				fixed_args...)...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
	_tuple_return geteach_ta_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		const _tuple_args& tuple_args,
		std::index_sequence<_index...>,
		_fixed_args&&... fixed_args)
	{
		return {
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				tuple_args,
				std::forward<_fixed_args>(fixed_args)...)...
		};
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		typename... _fixed_args,
		std::size_t... _index>
	_tuple_return geteach_ta_fa_indexed(
		_functor&& functor,
		_tuple&& tuple,
		_tuple_args&& tuple_args,
		std::index_sequence<_index...>,
		const _fixed_args&... fixed_args)
	{
		return {
			functor(
				std::get<_index>(std::forward<_tuple>(tuple)),
				std::forward<_tuple_args>(tuple_args),
				(fixed_args)...)...
		};
	}
#pragma endregion
	
#pragma endregion

#pragma region Helpers

#pragma region no_return_no_args
	template<
		typename _functor,
		typename _tuple,
		std::size_t _capacity>
	void foreach(
		_tuple& tuple)
	{
		foreach_na_indexed<
			_functor,
			_tuple>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::make_index_sequence<_capacity>{}
		);
	}
#pragma endregion

#pragma region no_tuple_return_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity>
	void foreach(
		_tuple& tuple,
		_tuple_args&& tuple_args)
	{
		foreach_ta_indexed<
			_functor,
			_tuple,
			_tuple_args>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{}
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity>
	void foreach(
		_tuple& tuple,
		_tuple_args& tuple_args)
	{
		foreach_ta_indexed<
			_functor,
			_tuple,
			_tuple_args>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{}
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity>
	void foreach(
		_tuple& tuple,
		const _tuple_args& tuple_args)
	{
		foreach_ta_indexed<
			_functor,
			_tuple,
			_tuple_args>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			tuple_args,
			std::make_index_sequence<_capacity>{}
		);
	}
#pragma endregion

#pragma region no_return_fixed_args
	template<
		typename _functor,
		typename _tuple,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		_fixed_args&&... fixed_args)
	{
		foreach_fa_indexed<
			_functor,
			_tuple,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		_fixed_args&... fixed_args)
	{
		foreach_fa_indexed<
			_functor,
			_tuple,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		const _fixed_args&... fixed_args)
	{
		foreach_fa_indexed<
			_functor,
			_tuple,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::make_index_sequence<_capacity>{},
			fixed_args...
		);
	}
#pragma endregion

#pragma region no_tuple_return_args_and_fixed_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		_tuple_args&& tuple_args,
		_fixed_args&&... fixed_args)
	{
		foreach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		_tuple_args& tuple_args,
		_fixed_args&&... fixed_args)
	{
		foreach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		_tuple_args&& tuple_args,
		_fixed_args&... fixed_args)
	{
		foreach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		_tuple_args&& tuple_args,
		const _fixed_args&... fixed_args)
	{
		foreach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			fixed_args...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		_tuple_args& tuple_args,
		const _fixed_args&... fixed_args)
	{
		foreach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			fixed_args...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		const _tuple_args& tuple_args,
		_fixed_args&&... fixed_args)
	{
		foreach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			tuple_args,
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	void foreach(
		_tuple& tuple,
		const _tuple_args& tuple_args,
		_fixed_args&... fixed_args)
	{
		foreach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			tuple_args,
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}
#pragma endregion

#pragma region return_no_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		std::size_t _capacity>
	_tuple_return geteach(
			_tuple& tuple)
	{
		return geteach_na_indexed<
			_functor,
			_tuple,
			_tuple_return>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::make_index_sequence<_capacity>{}
		);
	}
#pragma endregion

#pragma region return_tuple_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity>
	_tuple_return geteach(
		_tuple& tuple,
		_tuple_args&& tuple_args)
	{
		return geteach_ta_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{}
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity>
	_tuple_return geteach(
		_tuple& tuple,
		_tuple_args& tuple_args)
	{
		return geteach_ta_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{}
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity>
	_tuple_return geteach(
		_tuple& tuple,
		const _tuple_args& tuple_args)
	{
		return geteach_ta_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			tuple_args,
			std::make_index_sequence<_capacity>{}
		);
	}
#pragma endregion

#pragma region return_fixed_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		_fixed_args&&... fixed_args)
	{
		return geteach_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		_fixed_args&... fixed_args)
	{
		return geteach_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		const _fixed_args&... fixed_args)
	{
		return geteach_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::make_index_sequence<_capacity>{},
			fixed_args...
		);
	}
#pragma endregion

#pragma region return_tuple_args_and_fixed_args
	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		_tuple_args&& tuple_args,
		_fixed_args&&... fixed_args)
	{
		return geteach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		_tuple_args& tuple_args,
		_fixed_args&&... fixed_args)
	{
		return geteach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		_tuple_args&& tuple_args,
		_fixed_args&... fixed_args)
	{
		return geteach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		_tuple_args&& tuple_args,
		const _fixed_args&... fixed_args)
	{
		return geteach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			fixed_args...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		_tuple_args& tuple_args,
		const _fixed_args&... fixed_args)
	{
		return geteach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			std::forward<_tuple_args>(tuple_args),
			std::make_index_sequence<_capacity>{},
			fixed_args...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		const _tuple_args& tuple_args,
		_fixed_args&&... fixed_args)
	{
		return geteach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			tuple_args,
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}

	template<
		typename _functor,
		typename _tuple,
		typename _tuple_return,
		typename _tuple_args,
		std::size_t _capacity,
		typename... _fixed_args>
	_tuple_return geteach(
		_tuple& tuple,
		const _tuple_args& tuple_args,
		_fixed_args&... fixed_args)
	{
		return geteach_ta_fa_indexed<
			_functor,
			_tuple,
			_tuple_return,
			_tuple_args,
			_fixed_args...>
		(
			_functor(),
			std::forward<_tuple>(tuple),
			tuple_args,
			std::make_index_sequence<_capacity>{},
			std::forward<_fixed_args>(fixed_args)...
		);
	}
#pragma endregion

#pragma endregion

}

namespace functors {
	//Calls operator++()
	struct increment {
		template<
			typename _t>
		void operator()(
			_t&& t)
		{
			t++;
		}
	};

	//Calls operator--()
	struct decrement {
		template<
			typename _t>
		void operator()(_t&& t)
		{
			t--;
		}
	};

	//Calls delete
	struct erase {
		template<typename _t>
		void operator()(
			_t&& t)
		{
			delete t;
		}
	};

	//Calls delete[]
	struct erase_array {
		template<
			typename _t>
		void operator()(
			_t&& t)
		{
			delete[] t;
		}
	};

	//Calls operator=(const _t&)
	struct assign {
		template<
			typename _t>
		void operator()(
			_t&& t,
			const _t& d)
		{
			t = d;
		}
	};

	//Returns t[index]
	struct index {
		template<
			typename _t>
			_t operator()(
				_t&& t,
				std::size_t index)
		{
			return t[index];
		}
	};

	//Calls operator*()
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