#pragma once

#include <string>

namespace meta
{
	template<typename _t>
	struct tag {};

namespace internal
{
	template <typename _m>
	struct _ptrtype // https://stackoverflow.com/a/22851790
	{
		template <typename _t, typename _internal>
		static _internal get_type(_internal _t::*);
		typedef decltype(get_type(static_cast<_m>(nullptr))) type;
	};
}

	template<auto _m>
	using ptrtype = typename internal::_ptrtype<decltype(_m)>::type;

	template<typename _t>
	std::string type_name(tag<_t>);

	template<typename _t>
	std::string type_name(tag<_t>)
	{
		return typeid(_t).name();
	}

	template<typename _t>
	size_t type_hash(tag<_t>)
	{
		return typeid(_t).hash_code();
	}

	struct type_info
	{
		std::string m_name;
		size_t m_hash;
		size_t m_size;
	};

namespace internal
{
	template<typename _t>
	static type_info* make_type_info()
	{
		type_info* info = new type_info();
		info->m_name = type_name(tag<_t>{});
		info->m_hash = type_hash(tag<_t>{});
		info->m_size = sizeof(_t);
		
		return info;
	}
}

	// static store for type information, note dll bounds
	template<typename _t>
	static type_info* get_type_info()
	{
		static type_info* info = internal::make_type_info<_t>();
		return info;
	}
}