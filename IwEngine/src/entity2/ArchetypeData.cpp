#include "iw/entity2/ArchetypeData.h"

namespace IwEntity2 {
	bool ArchetypeData::operator==(
		const ArchetypeData& other) const
	{
		if (Count() != other.Count()) {
			return false;
		}

		auto itr  = m_layout.begin();
		auto oitr = other.m_layout.begin();
		for (; itr != m_layout.end(); itr++, oitr++) {
			if (itr.sparse_index() != itr.sparse_index()) {
				return false;
			}
		}

		return true;
	}

	bool ArchetypeData::EqualWith(
		ArchetypeData& other,
		Component id) const
	{
		if (Count() != 1 + other.Count()) {
			return false;
		}

		auto itr  = m_layout.begin();
		auto end  = m_layout.end();
		auto oitr = other.m_layout.begin();
		auto oend = other.m_layout.end();
		for (; itr != end && oitr != oend; itr++, oitr++) {
			if (itr.sparse_index() == id) {
				continue;
			}

			else if (itr.sparse_index() != oitr.sparse_index()) {
				return false;
			}
		}

		return true;
	}

	bool ArchetypeData::EqualWithout(
		ArchetypeData& other,
		Component id) const
	{
		if (Count() !=  other.Count() - 1) {
			return false;
		}

		auto itr  = m_layout.begin();
		auto end  = m_layout.end();
		auto oitr = other.m_layout.begin();
		auto oend = other.m_layout.end();
		for (;  oitr != oend; oitr++) {
			if (oitr.sparse_index() == id) {
				continue;
			}

			else if (itr.sparse_index() != oitr.sparse_index()) {
				return false;
			}

			if (itr != end) {
				itr++;
			}
		}

		return true;
	}

	void ArchetypeData::AddComponent(
		Component id,
		size_t size)
	{
		m_layout.emplace(id, size);
		m_layout.sort();
	}

	void ArchetypeData::RemoveComponent(
		Component id)
	{
		m_layout.erase(id);
	}

	bool ArchetypeData::HasComponent(
		Component id) const
	{
		return m_layout.contains(id);
	}

	size_t ArchetypeData::ComponentSize(
		Component id) const
	{
		return m_layout.at(id);
	}

	size_t ArchetypeData::SizeBefore(
		Component id) const
	{
		size_t size = 0;

		for (auto itr = m_layout.begin(); itr != m_layout.end(); itr++) {
			if (id == itr.sparse_index()) {
				break;
			}

			size += *itr;
		}

		return size;
	}

	size_t ArchetypeData::SizeAfter(
		Component id) const
	{
		size_t size = 0;
		bool after = false;
		for (auto itr = m_layout.begin(); itr != m_layout.end(); itr++) {
			if (after) {
				size += *itr;
			}
			
			if (id == itr.sparse_index()) {
				after = true;
			}
		}

		return size;
	}

	size_t ArchetypeData::Size() const {
		size_t size = 0;

		for (size_t s : m_layout) {
			size += s;
		}

		return size;
	}

	size_t ArchetypeData::Count() const {
		return m_layout.size();
	}

	bool ArchetypeData::Empty() const {
		return Count() == 0;
	}

	void ArchetypeData::Reset() {
		m_layout.clear();
	}
}
