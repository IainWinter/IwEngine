#pragma once

#include <string>

namespace meta
{
	// allows would be funcs with no args the abilityto compile an overload template
	// by passing this as a (nameless) argument

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
	std::string type_name(tag<_t>)
	{
		return typeid(_t).name();
	}
	
	struct type_info
	{
		std::string m_name;
		size_t m_size;

		bool m_is_floating;
		bool m_is_integral;
		
		bool m_is_structure;
	};

namespace internal
{
	template<typename _t>
	type_info* make_type_info()
	{
		type_info* info = new type_info();
		info->m_name = type_name(tag<_t>{});
		info->m_size = sizeof(_t);
		info->m_is_floating  = std::is_floating_point<_t>::value;
		info->m_is_integral  = std::is_integral<_t>::value;
		info->m_is_structure = !info->m_is_floating && !info->m_is_integral;

		return info;
	}
}

	// static store for type information, note dll bounds
	
	template<typename _t>
	type_info* get_type_info()
	{
		static type_info* info = internal::make_type_info<_t>();
		return info;
	}
}