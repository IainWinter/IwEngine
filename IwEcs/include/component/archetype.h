#pragma once

#include "type/type_group.h"

namespace iwent {
	using archetype = size_t;

	constexpr size_t archetype_size = sizeof(archetype) * 8;

	archetype add_type(
		archetype& a,
		size_t type_id)
	{
		return a |= 1UL << type_id;
	}

	archetype remove_type(
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
		return a & 1UL << offset == true;
	}

	bool is_empty(
		archetype a)
	{
		return a == false;
	}

	bool has_any(
		archetype a,
		archetype b)
	{
		return (a & b) != false;
	}

	template<
		typename _type_group,
		typename _t>
	archetype add_type(
		archetype& a)
	{
		return add_type(a, type_id<_type_group, _types>());
	}

	template<
		typename _type_group,
		typename _t>
	archetype remove_type(
		archetype& a)
	{
		return has_type(a, type_id<_type_group, _types>());
	}

	template<
		typename _type_group,
		typename _t>
	archetype has_type(
		archetype a)
	{
		return has_type(a, type_id<_type_group, _types>());
	}

	template<
		typename _type_group,
		typename... _types>
	archetype make_archetype() {
		return ((archetype(1) << type_id<_type_group, _types>()) | ...);
	}

	template<
		typename _type_group,
		typename _t>
	size_t type_id() {
		return iwutil::type_group<_type_group>::type<_t>;
	}
}
