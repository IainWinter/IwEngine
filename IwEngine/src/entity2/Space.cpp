#include "iw/entity2/Space.h"

// Archtype has to be collection of 

namespace IwEntity2 {
	Entity Space::CreateEntity() {
		return m_entities.CreateEntity();
	}

	bool Space::DestroyEntity(
		Entity entity)
	{
		EntityData& data = m_entities.GetEntityData(entity);
		if (data.Alive) {
			ComponentArray* ca = GetComponentArray(data.Archetype);
			ca->DestroyComponents(entity);
		}

		return m_entities.DestroyEntity(entity);
	}

	bool Space::EntityExists(
		Entity entity)
	{
		return m_entities.EntityExists(entity);
	}

	void* Space::CreateComponent(
		Entity entity,
		size_t componentSize,
		Component componentId)
	{
		if (!EntityExists(entity)) {
			return nullptr;
		}

		Archetype& archetype = m_entities.GetEntityData(entity).Archetype;

		size_t csize = 0;
		void*  cptr  = nullptr;

		ComponentArray* ca = GetComponentArray(archetype);
		if (ca) {
			csize = ca->ArchetypeSize();
			cptr  = ca->GetComponents(entity);
		}

		m_archetypes.AddComponent(archetype, componentId, componentSize);

		ComponentArray& newca = EnsureComponentArray(
			archetype, componentSize + csize);

		void* components = newca.CreateComponents(entity);

		if (cptr) {
			memmove(components, cptr, csize); // temp
			ca->DestroyComponents(entity);
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

		Archetype      archetype = m_entities.GetEntityData(entity).Archetype;
		ArchetypeData& archedata = m_archetypes.GetArchetypeData(archetype);

		ComponentArray* ca = GetComponentArray(archetype);

		// If entity only has 1 component no need to copy data
		if (archedata.Count() == 1) {
			ca->DestroyComponents(entity);
			archedata.RemoveComponent(componentId);
		}

		else {
			void* oldcomponents = ca->GetComponents(entity);

			size_t size   = archedata.Size();
			size_t before = archedata.SizeBefore(componentId);
			size_t after  = archedata.SizeAfter(componentId);

			m_archetypes.RemoveComponent(archetype, componentId);
			
			ComponentArray& newca = EnsureComponentArray(archetype, archedata.Size());

			void* components = newca.CreateComponents(entity);
			
			// temp
			memmove(components, oldcomponents, before);
			memmove((char*)components + before, (char*)oldcomponents + size - before - after, after);

			ca->DestroyComponents(entity);
		}

		return true;
	}

	void* Space::GetComponent(
		Entity entity,
		Component componentId)
	{
		Archetype      archetype = m_entities.GetEntityData(entity).Archetype;
		ArchetypeData& archedata = m_archetypes.GetArchetypeData(archetype);

		ComponentArray* ca = GetComponentArray(archetype);

		void* components = ca->GetComponents(entity);

		size_t before = archedata.SizeBefore(componentId);

		return (char*)components + before;
	}

	ComponentArray* Space::GetComponentArray(
		Archetype archetype)
	{
		auto itr = m_components.find(archetype);
		if (itr != m_components.end()) {
			return &itr->second;
		}

		return nullptr;
	}

	ComponentArray& Space::EnsureComponentArray(
		Archetype archetype,
		size_t archetypeSize)
	{
		ComponentArray* ca = GetComponentArray(archetype);
		if (!ca) {
			ca = &m_components.emplace(archetype, ComponentArray(
				6 * 1024 * 1024, archetypeSize)).first->second;
		}

		return *ca;
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
}
