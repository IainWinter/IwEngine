#pragma once

#include "iwecs.h"
#include "archtype.h"

namespace iwecs {
	class icomponent_data {
	public:
		virtual ~icomponent_data() {}

		virtual entity create_entity() = 0;
		//getting iterator
	};

	template<typename... _components_t>
	class component_data : public icomponent_data {
	private:
		using archtype_t = archtype<_components_t...>;
		using chunk_t = chunk<archtype_t>;

		std::vector<chunk_t> m_chunks;
		std::size_t m_free_index;

		void add_chunk() {
			m_chunks.push_back(chunk_t());
			m_free_index = m_chunks.size() - 1;
		}

		chunk_t& ensure_free_chunk() {
			if (m_free_index == -1) {
				add_chunk();
			}

			if (m_chunks.size() - 1 == m_free_index) {

			}

			m_chunks[m_working_chunk]
		}
	public:
		component_data()
			: m_working_chunk(-1) {}

		~component_data() {}

		entity create_entity() {

		}
	};
}