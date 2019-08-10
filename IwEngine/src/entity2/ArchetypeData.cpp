#include "iw/entity2/ArchetypeData.h"

namespace IwEntity2 {
	bool ArchetypeData::operator==(
		const ArchetypeData& other) const
	{
		if (Count() != other.Count()) {
			return false;
		}

		auto itr  = m_components.begin();
		auto oitr = other.m_components.begin();
		for (; itr != m_components.end(); itr++, oitr++) {
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

		auto itr  = m_components.begin();
		auto oitr = other.m_components.begin();
		for (; itr != m_components.end(); itr++, oitr++) {
			if (itr.sparse_index() == id) {
				continue;
			}

			else if (itr.sparse_index() != itr.sparse_index()) {
				return false;
			}
		}

		return true;
	}

	void ArchetypeData::AddComponent(
		Component id,
		size_t size)
	{
		m_components.emplace(id, size);
	}

	void ArchetypeData::RemoveComponent(
		Component id)
	{
		m_components.erase(id);
	}

	bool ArchetypeData::HasComponent(
		Component id) const
	{
		return m_components.contains(id);
	}

	size_t ArchetypeData::SizeBefore(
		Component id) const
	{
		size_t size = 0;

		for (auto itr = m_components.begin(); itr != m_components.end(); itr++) {
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
		for (auto itr = m_components.begin(); itr != m_components.end(); itr++) {
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

		for (size_t s : m_components) {
			size += s;
		}

		return size;
	}

	size_t ArchetypeData::Count() const {
		return m_components.size();
	}

	bool ArchetypeData::Empty() const {
		return Count() == 0;
	}

	void ArchetypeData::Reset() {
		m_components.clear();
	}
}
