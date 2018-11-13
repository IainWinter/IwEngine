#pragma once

#include <vector>
#include "chunk.h"
#include "IwUtil/identifiable.h"

namespace iwecs {
	class iarchtype : public iwutil::identifiable {
	public:
		virtual ~iarchtype() {}

		virtual void add_chunk() = 0;
		virtual entity create_entity() = 0;
	};

	template<typename... T>
	class archtype : public iarchtype {
	private:
		std::vector<chunk<T...>> m_chunks;
		std::size_t m_entities_pre_count;
	public:
		archtype(std::size_t entities_pre_chunk)
		  : m_chunks(),
			m_entities_pre_count(entities_pre_chunk) 
		{
			add_chunk();
		}

		~archtype() {}

		void add_chunk() override {
			m_chunks.push_back(chunk<T...>(m_entities_pre_count));
		}

		entity create_entity() override {
			return 0;
		}
	};
}