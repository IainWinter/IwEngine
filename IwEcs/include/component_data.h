#pragma once

#include "iwecs.h"
#include "archtype.h"

namespace iwecs {
	class icomponent_data {
	public:
		virtual ~icomponent_data() {}
		//function for getting iterator
	};

	template<typename... _components_t>
	class component_data : public icomponent_data {
	public:
		using archtype_t = archtype<_components_t...>;
		using chunk_t = chunk<archtype_t>;
	private:
		std::vector<chunk_t> m_chunks;
		chunk_t* m_working_chunk; //Cannot ensure that pointer is valid. Use ensure_free_chunk.

		void add_chunk() {
			m_chunks.push_back(chunk_t(12)); //TODO: test value
			m_working_chunk = &m_chunks.back();
		}

		chunk_t& ensure_free_chunk() {
			if (m_working_chunk == nullptr || m_working_chunk->is_full()) {
				add_chunk();
			}

			return *m_working_chunk;
		}
	public:
		entity create_entity(_components_t&&... args) {
			chunk_t& chunk = ensure_free_chunk();
			chunk.push_back(
				archtype_t(std::forward<_components_t>(args)...)
			);

			return 0;
		}
	};
}