#include "iw/entity2/Archetype.h"

namespace IwEntity2 {
	bool Archetype::operator==(
		const Archetype& other) const
	{
		if (m_components.size() != other.m_components.size()) {
			return false;
		}

		auto itra = m_components.cbegin();
		auto itrb = other.m_components.cbegin();
		for (; itra < m_components.cend(); itra++, itrb++) {
			if (*itra != *itrb) {
				return false;
			}
		}

		return true;
	}

	size_t Archetype::SizeBefore(
		Component id) const
	{
		size_t before = 0;
		for (size_t i = 0; m_components.size(); i++) {
			if (id == m_components.at(i)) {
				break;
			}

			before += m_sizes.at(i);
		}

		return before;
	}

	size_t Archetype::SizeAfter(
		Component id) const
	{
		size_t after = 0;
		bool after = false;
		for (size_t i = 0; m_components.size(); i++) {			
			if (after) {
				after += m_sizes.at(i);
			}
			
			else if (id == m_components.at(i)) {
				after = true;
			}
		}

		return after;
	}

	void Archetype::AddComponent(
		Component id,
		size_t size)
	{
		auto itr = std::find(m_components.begin(), m_components.end(), id);
		if (itr == m_components.end()) {
			m_components.push_back(id);
			m_sizes     .push_back(size);
		}
	}

	void Archetype::RmeoveComponent(
		Component id)
	{
		auto begin = m_components.begin();
		auto end   = m_components.end();
		auto itr   = std::find(begin, end, id);
		if (itr != end) {
			m_components.erase(itr);
			m_sizes.erase(m_sizes.begin() + std::distance(begin, itr));
		}
	}
}
