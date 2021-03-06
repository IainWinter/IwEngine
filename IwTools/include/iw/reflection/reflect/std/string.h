#pragma once

#include "iw/reflection/Reflect.h"
#include <string>

namespace iw {
namespace Reflect {
namespace detail {
	template<>
	inline const Class* GetClass(
		ClassTag<std::string>) 
	{
		static Class c = Class("std::string", sizeof(size_t) + sizeof(void*), 2);
		c.fields[0] = { "size",   GetType(TypeTag<size_t >()), 0 };
		c.fields[1] = { "string", GetType(TypeTag<char[1]>()), sizeof(size_t) }; // use ptr
		c.serialize = [](Serializer& serializer, const void* data) {
			const  std::string& string = *(std::string*)data;

			size_t size = string.size();
			serializer.SerializeField     (c.fields[0], &size);
			serializer.SerializeFieldArray(c.fields[1], iw::GetType<char>(), string.data(), size);
		};
		c.deserialize = [](Serializer& serializer, void* data) {
			std::string& string = *(std::string*)data;

			size_t size;
			serializer.DeserializeField(c.fields[0], &size);

			string.resize(size);
			serializer.DeserializeFieldArray(c.fields[1], iw::GetType<char>(), string.data(), size);

			string[size] = '\0';
		};
		return &c;
	}
}
}
}
