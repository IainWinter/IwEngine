#pragma once

#include <vector>
#include "chunk.h"
#include "IwUtil/identifiable.h"

namespace iwecs {
	class iarchtype : iwutil::identifiable {
	protected:
		std::size_t m_id;
	public:
		iarchtype(unsigned int id)
			: m_id(id) {}

		virtual ~iarchtype() {}

		std::size_t id() {
			return m_id;
		}
	};

	template<typename... T>
	class archtype : public iarchtype {
	private:
		std::vector<chunk<T...>> m_chunks;
		std::size_t m_entities_pre_count;
	public:
		archtype(std::size_t entities_pre_chunk)
		  : iarchtype(typeid(this).hash_code()),
			m_chunks(),
			m_entities_pre_count(entities_pre_chunk) 
		{
			add_chunk();
		}

		~archtype() {}

		void add_chunk() {
			m_chunks.push_back(chunk<T...>(m_entities_pre_count));
		}

		entity create_entity() {
			return 0;
		}
	};
}