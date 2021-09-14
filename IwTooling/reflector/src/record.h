#pragma once

struct record {
	std::string Name;
	std::vector<std::string> Bases;
	std::vector<std::pair<std::string, std::string>> Members;
};

#include "iw/util/reflection/Reflect.h"

IW_BEGIN_REFLECT

inline const Class* GetClass(ClassTag<record>)
{
	static Class c = Class("record", sizeof(record), 3);
	c.fields[0] = { "name",   GetType(TypeTag<std::string>()),                                      offsetof(record, Name)    };
	c.fields[1] = { "bases",  GetType(TypeTag<std::vector<std::string>>()),                         offsetof(record, Bases)   };
	c.fields[2] = { "fields", GetType(TypeTag<std::vector<std::pair<std::string, std::string>>>()), offsetof(record, Members) };

	return &c;
}

template<size_t _s>
inline const Class* GetClass(ClassTag<record[_s]>)
{
	static Class c = Class("record[]", sizeof(record[_s]), 3);
	c.fields[0] = { "name",   GetType(TypeTag<std::string>()),                                      offsetof(record, Name) };
	c.fields[1] = { "bases",  GetType(TypeTag<std::vector<std::string>>()),                         offsetof(record, Bases) };
	c.fields[2] = { "fields", GetType(TypeTag<std::vector<std::pair<std::string, std::string>>>()), offsetof(record, Members) };

	return &c;
}

IW_END_REFLECT
