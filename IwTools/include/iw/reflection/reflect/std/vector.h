#pragma once

#include "iw/reflection/Reflect.h"
#include <vector>

namespace iw {
namespace detail {
	template<
		typename _t>
	inline const Class* GetClass(
		ClassTag<std::vector<_t>>)
	{
		static Class c = Class("std::vector", sizeof(size_t) + sizeof(_t*), 2);
		c.fields[0] = { "size",  GetType(TypeTag<size_t>()), 0 };
		c.fields[1] = { "items", GetType(TypeTag<_t[1] >()), sizeof(size_t) }; // use ptr
		c.serialize = [](Serializer& serializer, const void* data) {
			const  std::vector<_t>& vector = *(std::vector<_t>*)data;

			size_t size = vector.size();
			serializer.SerializeField     (c.fields[0], &size);
			serializer.SerializeFieldArray(c.fields[1], iw::GetType<_t>(), vector.data(), size);
		};
		c.deserialize = [](Serializer& serializer, void* data) {
			std::vector<_t>& vector = *(std::vector<_t>*)data;

			size_t size;
			serializer.DeserializeField(c.fields[0], &size);

			vector.resize(size);
			serializer.DeserializeFieldArray(c.fields[1], iw::GetType<_t>(), vector.data(), size);
		};
		return &c;
	}
}
}
