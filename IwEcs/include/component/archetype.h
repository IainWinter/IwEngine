#pragma once

#include <initializer_list>
#include "type/type_group.h"

namespace iwent {
	using archetype = size_t;

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
	archetype add_type(
		archetype& a)
	{
		return add_type(a, iwutil::type_group<_type_group>::type<_t>);
	}

	template<
		typename _type_group,
		typename _t>
	archetype remove_type(
		archetype& a)
	{
		return has_type(a, iwutil::type_group<_type_group>::type<_t>);
	}

	template<
		typename _type_group,
		typename _t>
	archetype has_type(
		archetype a)
	{
		return has_type(a, iwutil::type_group<_type_group>::type<_t>);
	}

	template<
		typename _type_group,
		typename... _types>
	archetype make_archetype() {
		return ((archetype(1) << iwutil::type_group<_type_group>::type<_types>) | ...);
	}

	/*template<
		typename _type_group>
	struct archetype {
	private:
		using type_group = iwutil::type_group<archetype<_type_group>>;

		size_t m_id;

	public:
		archetype()
			: m_id(0)
		{}

		archetype(
			std::initializer_list<size_t> components)
			: m_id()
		{
			for (auto c : components) {
				m_id |= size_t(1) << c;
			}
		}

		bool operator<(
			const archetype& other) const
		{
			return m_id < other.m_id;
		}

		template<
			typename _t>
		void add_type() {
			m_id |= 1UL << type_group::type<_t>;
		}

		template<
			typename _t>
		void remove_type() {
			m_id &= ~(1UL << type_group::type<_t>);
		}

		template<
			typename _t>
		bool contains_type() {
			return m_id & 1UL << type_group::type<_t>;
		}

		bool contains_type(
			size_t id)
		{
			return m_id & 1UL << id;
		}

		template<
			typename _t>
		size_t id() {
			return type_group::type<_t>;
		}

		bool is_empty() {
			return m_id == 0;
		}
	};

	template<
		typename _type_group,
		typename... _types>
	archetype<_type_group> make_archetype() {
		return { iwutil::type_group<archetype<_type_group>>::type<_types>... };
	}*/
}
