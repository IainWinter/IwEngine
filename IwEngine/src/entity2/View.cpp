#include "iw/entity2/View.h"

namespace IwEntity2 {
	View::View(
		std::initializer_list<ComponentArray*> componentArrays)
		: m_componentArrays(new Iterator[componentArrays.size()])
		, m_componentCount(componentArrays.size())
	{
		auto itr = componentArrays.begin();
		for (size_t i = 0; i < m_componentCount; i++, itr++) {
			m_componentArrays[i] = (*itr)->begin();
		}
	}

	View::~View() {
		delete[] m_componentArrays;
	}
}
