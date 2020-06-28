#include "iw/entity/Prefab.h"
#include "iw/log/logger.h"

namespace iw {
namespace ECS {
	Prefab::Prefab()
		: m_memory(256)
	{}

	void Prefab::Add(
		iw::ref<Component> component,
		void* data)
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

		if (!data) {
			return;
		}

		if (m_memory.size() + component->Size > m_memory.capacity()) { // kinda annoying to have to do
			m_memory.resize(m_memory.capacity() * 2);
		}

		void* ptr = m_memory.alloc(component->Size);
		
		if (!ptr) {
			LOG_ERROR << "Prefab failed to allocate memory for component!";
			return;
		}

		component->DeepCopyFunc(ptr, data);

		m_offsets.push_back((char*)ptr - m_memory.memory());
	}

	bool Prefab::Set(
		iw::ref<Component> component,
		void* data)
	{
		if (!component) {
			LOG_WARNING << "Tried to set an empty component on a prefab!";
			return false;
		}

		if (!Has(component)) {
			LOG_WARNING << "Tried to set a component that doesn't exists in a prefab!";
			return false;
		}

		if (!data) {
			LOG_WARNING << "Tried to set a component to null in a prefab!";
			return false;
		}

		int index = 0;
		for (; index < ComponentCount(); index++) {
			if (m_components[index] = component) {
				break;
			}
		}

		component->DeepCopyFunc(GetComponentData(index), data);

		return true;
	}

	void Prefab::Remove(
		iw::ref<Component> component)
	{
		// needs to clear linear alloc and then realloc every component :(
		LOG_WARNING << "no impl";
	}

	bool Prefab::Has(
		iw::ref<Component> component)
	{
		return std::find(m_components.begin(), m_components.end(), component) != m_components.end();
	}

	const iw::ref<Component>& Prefab::GetComponent(
		unsigned index) const
	{
		return m_components.at(index);
	}

	void* Prefab::GetComponentData(
		unsigned index) const
	{
		return m_memory.memory() + m_offsets.at(index);
	}

	unsigned Prefab::ComponentCount() const {
		return m_components.size();
	}
}
}
