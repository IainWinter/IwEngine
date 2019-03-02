#pragma once

#include "type/type_group.h"

namespace iwent {
	using archetype = size_t;

	constexpr size_t archetype_size = sizeof(archetype) * 8;

	archetype& add_type(
		archetype& a,
		size_t type_id)
	{
		return a |= 1UL << type_id;
	}

	archetype& remove_type(
		archetype& a,
		size_t type_id)
	{
		return a &= ~(1UL << type_id);
	}

	bool has_type(
		archetype a,
		size_t type_id)
	{
		return a & 1UL << type_id;
	}

	bool is_active(
		archetype a,
		size_t offset)
	{
		return (a & 1UL << offset) == 1;
	}

	bool is_empty(
		archetype a)
	{
		return a == 0;
	}

	bool has_any(
		archetype a,
		archetype b)
	{
		return (a & b) != 0;
	}

	template<
		typename _type_group,
		typename _t>
	archetype& add_type(
		archetype& a)
	{
		return add_type(a, type_id<_type_group, _t>());
	}

	template<
		typename _type_group,
		typename _t>
	archetype& remove_type(
		archetype& a)
	{
		return remove_type(a, type_id<_type_group, _t>());
	}

	template<
		typename _type_group,
		typename _t>
	archetype has_type(
		archetype a)
	{
		return has_type(a, type_id<_type_group, _t>());
	}

	template<
		typename _type_group,
		typename... _t>
	archetype make_archetype() {
		if constexpr (sizeof...(_t) > 1) {
			return ((archetype(1) << type_id<_type_group, _t>()) | ...);
		}

		else {
			return archetype(1) << type_id<_type_group, _t...>();
		}
	}

	template<
		typename _type_group,
		typename... _t>
	size_t type_id() {
		return iwutil::type_group<_type_group>::type<_t...>;
	}
}
