#pragma once

#include "iw/reflection/Reflect.h"
#include <string>

namespace iw {
namespace detail {
	template<>
	inline const Class* GetClass(
		ClassTag<std::string>) 
	{
		static Class c = Class("std::string", sizeof(size_t) + sizeof(void*), 2);
		c.fields[0] = { "size",   GetType(TypeTag<size_t>()),          0 };
		c.fields[1] = { "string", GetType(TypeTag<char[1]>()), sizeof(size_t) }; // use ptr
		return &c;
	}
}
}
