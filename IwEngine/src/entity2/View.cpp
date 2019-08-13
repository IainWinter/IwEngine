#include "iw/entity2/View.h"

namespace IwEntity2 {
	View::View(
		std::vector<ComponentArray*>&& componentArrays)
		: m_componentArrays(std::move(componentArrays))
	{}

	View::Iterator::Iterator(
		std::vector<ComponentArray::Iterator>&& itrs)
		: m_itrs(std::move(itrs))
	{}

	View::Iterator View::begin() {
		std::vector<ComponentArray::Iterator> itrs(m_componentArrays.size());
		for (ComponentArray* ca : m_componentArrays) {
			itrs.push_back(ca->begin());
		}

		return Iterator(std::move(itrs));
	}

	View::Iterator View::end() {
		std::vector<ComponentArray::Iterator> itrs(m_componentArrays.size());
		for (ComponentArray* ca : m_componentArrays) {
			itrs.push_back(ca->end());
		}

		return Iterator(std::move(itrs));
	}
}
