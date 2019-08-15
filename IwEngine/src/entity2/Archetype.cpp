#include "iw/entity2/Archetype.h"

namespace IwEntity2 {
	bool Archetype::operator==(
		const Archetype& other) const
	{
		if (Count() != other.Count()) {
			return false;
		}

		auto itr  = m_layout.begin();
		auto oitr = other.m_layout.begin();
		for (; itr != m_layout.end(); itr++, oitr++) {
			if (itr->Type != itr->Type) {
				return false;
			}
		}

		return true;
	}

	bool Archetype::EqualWith(
		Archetype& other,
		ComponentType type) const
	{
		if (Count() != 1 + other.Count()) {
			return false;
		}

		auto itr  = m_layout.begin();
		auto end  = m_layout.end();
		auto oitr = other.m_layout.begin();
		auto oend = other.m_layout.end();
		for (; itr != end && oitr != oend; itr++, oitr++) {
			if (itr->Type == type) {
				continue;
			}

			else if (itr->Type != oitr->Type) {
				return false;
			}
		}

		return true;
	}

	bool Archetype::EqualWithout(
		Archetype& other,
		ComponentType type) const
	{
		if (Count() !=  other.Count() - 1) {
			return false;
		}

		auto itr  = m_layout.begin();
		auto end  = m_layout.end();
		auto oitr = other.m_layout.begin();
		auto oend = other.m_layout.end();
		for (;  oitr != oend; oitr++) {
			if (oitr->Type == type) {
				continue;
			}

			else if (itr->Type != oitr->Type) {
				return false;
			}

			if (itr != end) {
				itr++;
			}
		}

		return true;
	}

	void Archetype::AddComponent(
		ComponentType type,
		size_t size)
	{
		auto itr = m_layout.begin();
		while (itr != m_layout.end()) {
			if (itr->Type == type) {
			}

			++itr;
		}

		m_layout.emplace(itr, type, size);
	}

	void Archetype::RemoveComponent(
		ComponentType type)
	{

		auto itr = m_layout.begin();
		


		auto itr = std::find(m_layout.begin(), m_layout.end(), type);
		m_layout.erase(itr);
	}

	bool Archetype::HasComponent(
		ComponentType type) const
	{
		for (ComponentTypeInfo info : m_layout) {
			if (info.Type == type) {
				return true;
			}
		}

		return false;
	}

	size_t Archetype::ComponentSize(
		ComponentType type) const
	{
		return m_layout[type].Size;
	}

	size_t Archetype::SizeBefore(
		ComponentType type) const
	{
		size_t size = 0;

		for (auto itr = m_layout.begin(); itr != m_layout.end(); itr++) {
			if (type == itr->Type) {
				break;
			}

			size += itr->Size;
		}

		return size;
	}

	size_t Archetype::SizeAfter(
		ComponentType type) const
	{
		size_t size = 0;
		bool after = false;
		for (auto itr = m_layout.begin(); itr != m_layout.end(); itr++) {
			if (after) {
				size += itr->Size;
			}
			
			if (type == itr->Type) {
				after = true;
			}
		}

		return size;
	}

	size_t Archetype::Size() const {
		size_t size = 0;

		for (ComponentTypeInfo info : m_layout) {
			size += info.Size;
		}

		return size;
	}

	size_t Archetype::Count() const {
		return m_layout.size();
	}

	bool Archetype::Empty() const {
		return Count() == 0;
	}

	void Archetype::Reset() {
		m_layout.clear();
	}
}
