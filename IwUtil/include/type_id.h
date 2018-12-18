#pragma once

#include <cstddef>

namespace iwutil {
	class type_id {
	private:
		static std::size_t m_counter;

	public:
		template<
			typename _t>
		static std::size_t value() {
			static std::size_t id = m_counter++;
			return id;
		}

		template<
			typename _t,
			typename... _others_t>
		static std::size_t value() {
			static std::size_t id = m_counter + value<_others_t...>();
			m_counter++;

			return id;
		}
	};

	std::size_t type_id::m_counter = 1;
}