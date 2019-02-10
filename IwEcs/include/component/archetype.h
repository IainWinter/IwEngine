#pragma once

#include <initializer_list>
#include "type/type_group.h"

namespace iwecs {
	struct archetype {
	private:
		using type_group = iwutil::type_group<archetype>;

		size_t m_id;

	public:
		archetype(
			std::initializer_list<size_t> components)
			: m_id()
		{
			for (auto c : components) {
				m_id |= size_t(1) << c;
			}
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
	};

	template<
		typename... _t>
	archetype make_archetype() {
		return { iwutil::type_group<archetype>::type<_t>... };
	}
}