#include "iw/entity2/Space.h"

// Archtype has to be collection of 

namespace IwEntity2 {
	Entity Space::CreateEntity() {
		return m_entities.CreateEntity().Id;
	}

	bool Space::DestroyEntity(
		Entity entity)
	{
		bool success = m_entities.DestroyEntity(entity);
		if (success) {
			ComponentArray* ca = FindComponentArray(entity);
			ca->DestroyComponents(entity);
		}

		return success;
	}

	bool Space::EntityExists(
		Entity entity)
	{
		return m_entities.EntityExists(entity);
	}

	Component Space::GetComponentId(
		std::type_index&& type)
	{
		static int nextId = 0;
		auto itr = m_componentIds.find(type);
		if (itr == m_componentIds.end()) {
			itr = m_componentIds.emplace(type, nextId++).first;
		}

		return itr->second;
	}

	void* Space::CreateComponent(
		Entity entity,
		size_t componentSize,
		Component componentId)
	{
		if (!EntityExists(entity)) {
			return nullptr;
		}

		EntityData& data = m_entities.GetEntityData(entity);
		Archetype& arch  = data.Arch;

		size_t csize = 0;
		void*  cptr  = nullptr;

		ComponentArray* oldca = GetComponentArray(arch);
		if (oldca) {
			csize = oldca->ArchetypeSize();
			cptr = oldca->GetComponents(entity);
		}

		arch.AddComponent(componentId); //find how to hash multiable ints

		ComponentArray* newca = GetComponentArray(arch);
		if (!newca) {
			newca = CreateComponentArray(arch, csize + componentSize);
		}

		void* components = newca->CreateComponents(entity);

		if (cptr) {
			memmove(components, cptr, csize);
			oldca->DestroyComponents(entity);
		}

		return (char*)components + csize;
	}

	bool Space::DestroyComponent(
		Entity entity,
		Component componentId)
	{
		if (!EntityExists(entity)) {
			return false;
		}

		EntityData& data = m_entities.GetEntityData(entity);
		Archetype& arch  = data.Arch;
		
		ComponentArray* ca = GetComponentArray(arch);

		void* components = ca->GetComponents(entity);

		//use size before and after to find how to move components to new componentarray

		if (ca) {
			ca->DestroyComponents(entity);
		}

		return ca;
	}

	ComponentArray* Space::GetComponentArray(
		const Archetype& archetype)
	{
		auto itr = m_components.find(archetype);
		if (itr != m_components.cend()) {
			return &itr->second;
		}

		return nullptr;
	}

	ComponentArray* Space::FindComponentArray(
		Entity entity)
	{
		if (m_entities.EntityExists(entity)) {
			EntityData& data = m_entities.GetEntityData(entity);
			return GetComponentArray(data.Arch);
		}

		return nullptr;
	}

	ComponentArray* Space::CreateComponentArray(
		const Archetype& archetype,
		size_t archetypeSize)
	{
		return &m_components.emplace(archetype, 
			ComponentArray(1024, archetypeSize)).first->second;
	}
}
