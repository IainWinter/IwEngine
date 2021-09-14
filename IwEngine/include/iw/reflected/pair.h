#pragma once

#include "iw/util/reflection/Reflect.h"
#include <utility>

IW_BEGIN_REFLECT

template<
	typename _t1,
	typename _t2>
inline const Class* GetClass(
	ClassTag<std::pair<_t1, _t2>>)
{
	static Class c = Class("std::pair", sizeof(std::pair<_t1, _t2>), 2);
	c.fields[0] = { "first",  GetType(TypeTag<_t1>()), 0           };
	c.fields[1] = { "second", GetType(TypeTag<_t2>()), sizeof(_t1) };

	return &c;
}

template<
	typename _t1,
	typename _t2,
	size_t _s>
inline const Class* GetClass(
	ClassTag<std::pair<_t1, _t2>[_s]>)
{
	static Class c = Class("std::pair[]", sizeof(std::pair<_t1, _t2>[_s]), 2);
	c.fields[0] = { "first",  GetType(TypeTag<_t1>()), 0 };
	c.fields[1] = { "second", GetType(TypeTag<_t2>()), sizeof(_t1) };

	return &c;
}

IW_END_REFLECT
