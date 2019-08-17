#include "iw/entity2/Archetype.h"
#include <algorithm>

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
		if (Count() != other.Count() + 1) {
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
		if (Count() != other.Count() - 1) {
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
		ComponentTypeInfo info = { type, size };
		auto itr = std::upper_bound(m_layout.begin(), m_layout.end(), info, [](
			ComponentTypeInfo a,
			ComponentTypeInfo b)
		{
			return a.Type < b.Type;
		});

		m_layout.emplace(itr, std::move(info));
	}

	void Archetype::RemoveComponent(
		ComponentType type)
	{
		ComponentTypeInfo info = { type, 0 };
		auto itr = std::lower_bound(m_layout.begin(), m_layout.end(), info, [](
			ComponentTypeInfo a,
			ComponentTypeInfo b)
		{
			return a.Type < b.Type;
		});

		m_layout.erase(itr);
	}

	bool Archetype::HasComponent(
		ComponentType type) const
	{
		ComponentTypeInfo info = { type, 0 };
		bool success = std::binary_search(m_layout.begin(), m_layout.end(), info, [](
			ComponentTypeInfo a,
			ComponentTypeInfo b)
		{
			return a.Type < b.Type;
		});	

		return success;
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
