#pragma once





template<typename...>
class factory;

template<
	typename _t,
	typename... _others_t>
class factory<
	_t, 
	_others_t...>
	: factory<_others_t>...
{
public:
	using type      = _t;
	using pointer   = _t*;
	using reference = _t&;
};

template<
	typename _t>
class factory<
	_t>
{
private:

public:
	_t construct() {
		return _t();
	}


};