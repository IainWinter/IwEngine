#pragma once

#include "iwecs.h"
#include "archtype.h"
#include "chunk.h"

namespace iwecs {
	class icomponent_data {
	public:
		virtual ~icomponent_data() {}

		virtual bool destroy_components(entity entity) = 0;
	};

	template<typename... _components_t>
	class component_data : public icomponent_data {
	public:
		using archtype_t = archtype<_components_t...>;
		using chunk_t = chunk<archtype_t>;
	private:
		std::vector<chunk_t*> m_chunks; //prob not gonig to be the final data structure to use. Chunks should not be continguious in memory.
		chunk_t* m_working_chunk; //Cannot ensure that pointer is valid. Use ensure_free_chunk.

		void add_chunk() {
 			m_chunks.push_back(new chunk_t(640));
			m_working_chunk = m_chunks.back();
		}

		chunk_t& ensure_free_chunk() {
			if (m_working_chunk == nullptr || m_working_chunk->is_full()) {
				add_chunk();
			}

			return *m_working_chunk;
		}
	public:
		void attach_components(entity entity, _components_t&&... args) {
			chunk_t& chunk = ensure_free_chunk();
			chunk.insert(
				entity,
				archtype_t(std::forward<_components_t>(args)...)
			);
		}

		bool destroy_components(entity entity) {
			for (auto& chunk : m_chunks) {
				if (chunk->remove(entity)) {
					m_working_chunk = chunk;
					return true;
				}
			}

			return false;
		}
	};
}