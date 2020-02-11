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
		static Class c = Class("std::vector", sizeof(size_t) + sizeof(void*), 2);
		c.fields[0] = { "size",   GetType(TypeTag<size_t>()),        0 };
		c.fields[1] = { "iteams", GetType(TypeTag<_t[1]>()), sizeof(size_t) }; // use ptr
		return &c;
	}

	template<
		typename _t,
		typename _s>
	void Serialize(
		_s& stream,
		const std::vector<_t>& vector)
	{
		const iw::Type* type = GetType<_t>();

		Serialize<size_t>(stream, vector.size());

		if (type->isClass) {
			for (const _t& item : vector) {
				type->AsClass()->Serialize(stream, &item);
			}
		}

		else {
			for (const _t& item : vector) {
				type->Serialize(stream, &item);
			}
		}
	}

	template<
		typename _t,
		typename _s>
	void Deserialize(
		_s& stream,
		std::vector<_t>& vector)
	{
		const iw::Type* type = GetType<_t>();

		size_t size;
		Deserialize<size_t>(stream, size);

		vector.resize(size);

		if (type->isClass) {
			for (_t& item : vector) {
				type->AsClass()->Deserialize(stream, &item);
			}
		}

		else {
			for (_t& item : vector) {
				type->Deserialize(stream, &item);
			}
		}
	}
}
}
