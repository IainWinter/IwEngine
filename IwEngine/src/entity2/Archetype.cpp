#include "iw/entity2/Archetype.h"
#include <algorithm>

namespace IwEntity2 {
	//bool Archetype::operator==(
	//	const Archetype& other) const
	//{
	//	if (Count() != other.Count()) {
	//		return false;
	//	}

	//	auto itr  = m_layout.begin();
	//	auto oitr = other.m_layout.begin();
	//	for (; itr != m_layout.end(); itr++, oitr++) {
	//		if (itr->Type != itr->Type) {
	//			return false;
	//		}
	//	}

	//	return true;
	//}

	bool Archetype::EqualWith(
		Archetype& other,
		ComponentType type) const
	{
		if (Count() != other.Count() + 1) {
			return false;
		}

		for(size_t i = 0; i < Count)

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

	bool Archetype::HasComponent(
		ComponentType type) const
	{

	}

	size_t Archetype::ComponentSize(
		ComponentType type) const
	{

		return Layout[type].Size;
	}

	size_t Archetype::SizeBefore(
		ComponentType type) const
	{

	}

	size_t Archetype::SizeAfter(
		ComponentType type) const
	{

	}

	Archetype* Archetype::MakeArchetype(
		std::initializer_list<ComponentTypeInfo> layout)
	{
		const size_t size
			= ArchetypeHeaderSize
			+ sizeof(ComponentTypeInfo)
			* layout.size();

		Archetype* archetype = (Archetype*)malloc(size);

		size_t i = 0;
		auto   itr = layout.begin();
		while (itr != layout.end()) {
			archetype->Layout[i] = {
				itr->Type,
				itr->Size,
				archetype->Size
			};

			archetype->Size += itr->Size;

			i++;
			itr++;
		}
	}
}
