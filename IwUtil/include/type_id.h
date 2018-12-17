#pragma once

namespace iwutil {
	class type_id {
	private:
		static size_t m_counter;

	public:
		template<
			typename _t>
		static size_t value() {
			static size_t id = m_counter++;
			return id;
		}

		template<
			typename _t,
			typename... _others_t>
		static size_t value() {
			static size_t id = m_counter + value<_others_t...>();
			m_counter++;

			return id;
		}
	};

	size_t type_id::m_counter = 1;
}