#pragma once

#include "test.h"
#include "iw/util/reflection/Reflect.h"
//
IW_BEGIN_REFLECT
//
//inline const Class* GetClass(ClassTag<record>)
//{
//	static Class c = Class("record", sizeof(record), 3);
//	c.fields[0] = { "name",   GetType(TypeTag<std::string>()),                                      offsetof(record, Name)    };
//	c.fields[1] = { "bases",  GetType(TypeTag<std::vector<std::string>>()),                         offsetof(record, Bases)   };
//	c.fields[2] = { "fields", GetType(TypeTag<std::vector<std::pair<std::string, std::string>>>()), offsetof(record, Fields) };
//
//	return &c;
//}
//
//template<size_t _s>
//inline const Class* GetClass(ClassTag<record[_s]>)
//{
//	static Class c = Class("record[]", sizeof(record[_s]), 3);
//	c.fields[0] = { "name",   GetType(TypeTag<std::string>()),                                      offsetof(record, Name) };
//	c.fields[1] = { "bases",  GetType(TypeTag<std::vector<std::string>>()),                         offsetof(record, Bases) };
//	c.fields[2] = { "fields", GetType(TypeTag<std::vector<std::pair<std::string, std::string>>>()), offsetof(record, Fields) };
//
//	return &c;
//}
//
template<typename _t, size_t _s, int _i, test_enum _d>
inline const Class* GetClass(ClassTag<test_base<_t, _s, _i, _d>>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("test_base<_t, _s, _i, _d>", sizeof(test_base<_t, _s, _i, _d>), 3);

		size_t offset = 0;

		c->fields[0] = { "base_x", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[1] = { "base_y", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[2] = { "base_t", GetType(TypeTag<_t>()), offset, false };
	}

	return c;
}

template<
	typename _t, size_t _s, int _i, test_enum _d,
	size_t _iw_array_size>
	inline const Class* GetClass(ClassTag<test_base<_t, _s, _i, _d>[_iw_array_size]>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class(get_array_type_name("test_base<_t, _s, _i, _d>", _iw_array_size), sizeof(test_base<_t, _s, _i, _d>[_iw_array_size]), 3);

		size_t offset = 0;

		c->fields[0] = { "base_x", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[1] = { "base_y", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[2] = { "base_t", GetType(TypeTag<_t>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<test_int>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("test_int", sizeof(test_int), 4);

		size_t offset = 0;

		c->fields[0] = { "test_base<int, 2, 1, D2>", GetClass(ClassTag<test_base<int, 2, 1, D2>>()), offset, true };
		offset += sizeof(test_base<int, 2, 1, D2>);

		c->fields[1] = { "x", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[2] = { "y", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[3] = { "xy", GetType(TypeTag<int>()), offset, false };
	}

	return c;
}

template<
	size_t _iw_array_size>
	inline const Class* GetClass(ClassTag<test_int[_iw_array_size]>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class(get_array_type_name("test_int", _iw_array_size), sizeof(test_int[_iw_array_size]), 4);

		size_t offset = 0;

		c->fields[0] = { "test_base<int, 2, 1, D2>", GetClass(ClassTag<test_base<int, 2, 1, D2>>()), offset, true };
		offset += sizeof(test_base<int, 2, 1, D2>);

		c->fields[1] = { "x", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[2] = { "y", GetType(TypeTag<float>()), offset, false };
		offset += sizeof(float);

		c->fields[3] = { "xy", GetType(TypeTag<int>()), offset, false };
	}

	return c;
}

template<>
inline const Class* GetClass(ClassTag<test_float>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class("test_float", sizeof(test_float), 4);

		size_t offset = 0;

		c->fields[0] = { "test_base<float, 5, 2, D1>", GetClass(ClassTag<test_base<float, 5, 2, D1>>()), offset, true };
		offset += sizeof(test_base<float, 5, 2, D1>);

		c->fields[1] = { "x", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[2] = { "y", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[3] = { "xy", GetType(TypeTag<float>()), offset, false };
	}

	return c;
}

template<
	size_t _iw_array_size>
	inline const Class* GetClass(ClassTag<test_float[_iw_array_size]>)
{
	static Class* c = nullptr;

	if (c == nullptr)
	{
		c = new Class(get_array_type_name("test_float", _iw_array_size), sizeof(test_float[_iw_array_size]), 4);

		size_t offset = 0;

		c->fields[0] = { "test_base<float, 5, 2, D1>", GetClass(ClassTag<test_base<float, 5, 2, D1>>()), offset, true };
		offset += sizeof(test_base<float, 5, 2, D1>);

		c->fields[1] = { "x", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[2] = { "y", GetType(TypeTag<int>()), offset, false };
		offset += sizeof(int);

		c->fields[3] = { "xy", GetType(TypeTag<float>()), offset, false };
	}

	return c;
}


IW_END_REFLECT



