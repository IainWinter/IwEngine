#include "iw/entity2/ComponentArray.h"
#include "iw/entity2/ArchetypeData.h"

namespace IwEntity2 {
	ComponentArray::ComponentArray(
		size_t pageSize,
		const ArchetypeData& archetypeData)
		: m_archetype(archetypeData)
		, m_pool(pageSize, archetypeData.Size())
	{}

	void* ComponentArray::CreateComponents(
		Entity entity)
	{
		auto itr = m_entities.find(entity);
		if (itr == m_entities.end()) {
			void* components = m_pool.alloc();
			m_entities.emplace(entity, components);

			return components;
		}

		return nullptr;
	}

	bool ComponentArray::DestroyComponents(
		Entity entity)
	{
		auto itr = m_entities.find(entity);
		if (itr != m_entities.end()) {
			void* ptr = m_entities.at(entity);
			m_entities.erase(entity);
			m_pool.free(ptr);

			return true;
		}

		return false;
	}

	void* ComponentArray::GetComponents(
		Entity entity)
	{
		auto itr = m_entities.find(entity);
		if (itr != m_entities.end()) {
			return m_entities.at(entity);
		}

		return nullptr;
	}

	void* ComponentArray::CopyInto(
		ComponentArray& source,
		Entity entity)
	{
		auto& sarchetypeData = source.Archetype();
		void* dcomponents    = GetComponents(entity);
		void* scomponents    = source.GetComponents(entity);

		//sarchetypeData.SizeBefore();

		//for (auto itr = dlayout.begin(); itr != dlayout.end(); itr++) {
		//	size_t ssize = slayout.at
		//}

		return nullptr;
	}

	ComponentArray::Iterator ComponentArray::begin() {
		return Iterator(m_entities.begin(), &m_archetype);
	}

	ComponentArray::Iterator ComponentArray::end() {
		return Iterator(m_entities.end(), &m_archetype);
	}

	ComponentArray::ConstIterator ComponentArray::begin() const {
		return ConstIterator(m_entities.begin(), &m_archetype);
	}

	ComponentArray::ConstIterator ComponentArray::end() const {
		return ConstIterator(m_entities.end(), &m_archetype);
	}
}
