#include "iw/entity2/View.h"

namespace IwEntity2 {
	View::View(
		std::vector<ComponentArray*>&& componentArrays)
		: m_componentArrays(std::move(componentArrays))
	{}

	View::Iterator::Iterator(
		std::vector<ComponentArray::Iterator>&& itrs)
		: m_current(0)
		, m_itrs(std::move(itrs))
	{}

	View::Iterator View::begin() {	
		size_t count = m_componentArrays.size();

		std::vector<ComponentArray::Iterator> itrs;
		itrs.reserve(count);

		for (size_t i = 0; i < count; i++) {
			itrs.push_back(m_componentArrays[i]->begin());
		}

		return Iterator(std::move(itrs));
	}

	View::Iterator View::end() {
		size_t count = m_componentArrays.size();

		std::vector<ComponentArray::Iterator> itrs;
		itrs.reserve(count);

		for (size_t i = 0; i < count; i++) {
			itrs.push_back(m_componentArrays[i]->end());
		}

		return Iterator(std::move(itrs));
	}
}
