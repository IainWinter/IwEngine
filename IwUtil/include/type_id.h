#pragma once

namespace iwutil {
	class type_id {
	private:
		static size_t m_counter;

	public:
		template<typename T>
		static size_t value()
		{
			static size_t id = m_counter++;
			return id;
		}
	};

	size_t type_id::m_counter = 1;
}