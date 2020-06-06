#include "iw/entity/Prefab.h"
#include "iw/log/logger.h"

namespace iw {
namespace ECS {
	Prefab::Prefab()
		: m_memory(256)
	{}

	void Prefab::Add(
		iw::ref<Component> component,
		void* prefab)
	{
		if (!component) {
			LOG_WARNING << "Tried to add an empty component to a prefab!";
			return;
		}

		if (Has(component)) {
			LOG_WARNING << "Tried to add two of the same component to a prefab!";
			return;
		}

		m_components.push_back(component);

		if (m_memory.size() + component->Size > m_memory.capacity()) { // kinda annoying to have to do
			m_memory.resize(m_memory.capacity() * 2);
		}

		void* compData = m_memory.alloc(component->Size);
		
		if (prefab) {
			memcpy(compData, prefab, component->Size);
		}

		m_componentData.push_back(compData);
	}

	bool Prefab::Set(
		iw::ref<Component> component,
		void* prefab)
	{
		if (!component) {
			LOG_WARNING << "Tried to add an empty component to a prefab!";
			return false;
		}

		if (Has(component)) {
			LOG_WARNING << "Tried to add two of the same component to a prefab!";
			return false;
		}

		if (prefab) {
			auto itr = std::find(m_components.begin(), m_components.end(), component);
			size_t index = std::distance(m_components.begin(), itr);

			void* compData = m_componentData.at(index);

			memcpy(compData, prefab, component->Size);
		}

		return true;
	}

	void Prefab::Remove(
		iw::ref<Component> component)
	{

	}

	bool Prefab::Has(
		iw::ref<Component> component)
	{
		return false;
	}

	const std::vector<iw::ref<Component>>& Prefab::Components() const {
		return m_components;
	}

	const std::vector<void*>& Prefab::ComponentData() const {
		return m_componentData;
	}
}
}
