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

		Archetype& archetype   = m_entities.GetEntityData(entity).Archetype;

		ComponentArray* oldca = GetComponentArray(archetype);
		
		m_archetypes.AddComponent(archetype, componentId, componentSize);

		ComponentArray& newca = EnsureComponentArray(archetype);
		
		void* components = newca.CreateComponents(entity);


		if (oldca) {
			size_t olds  = oldca->Archetype().Size();
			size_t newsb = newca.Archetype().SizeBefore(componentId);
			size_t newsa = newca.Archetype().SizeAfter(componentId);

			char* newcomponents = (char*)components;
			char* oldcomponents = (char*)oldca->GetComponents(entity);
			
			// New component is at the end
			if (newsa == 0) {
				memmove(newcomponents, oldcomponents, newsb);
			}

			// New component is at the start
			else if (newsb == 0) {
				memmove(newcomponents + componentSize, oldcomponents, olds);
			}

			else {
				memmove(newcomponents, oldcomponents, newsb);
				memmove(newcomponents + newsb + componentSize, oldcomponents + newsb, newsa);
			}

			return newcomponents + newsb;
		}

		return components;
	}

	bool Space::DestroyComponent(
		Entity entity,
		Component componentId)
	{
		if (!EntityExists(entity)) {
			return false;
		}

		Archetype& archetype = m_entities.GetEntityData(entity).Archetype;

		ComponentArray* oldca = GetComponentArray(archetype);

		m_archetypes.RemoveComponent(archetype, componentId);

		if (archetype != 0) {
			ComponentArray& newca = EnsureComponentArray(archetype);

			size_t olds  = oldca->Archetype().Size();
			size_t oldsb = oldca->Archetype().SizeBefore(componentId);
			size_t oldsa = oldca->Archetype().SizeAfter(componentId);
			size_t oldcs = oldca->Archetype().ComponentSize(componentId);

			char* newcomponents = (char*)newca.CreateComponents(entity);
			char* oldcomponents = (char*)oldca->GetComponents(entity);

			memmove(newcomponents, oldcomponents, oldsb);
			memmove(newcomponents + oldsb, oldcomponents + oldsb + oldcs, oldsa);
		}

		oldca->DestroyComponents(entity);

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
			return *itr;
		}

		return nullptr;
	}

	ComponentArray& Space::EnsureComponentArray(
		Archetype archetype)
	{
		ComponentArray* ca = GetComponentArray(archetype);
		if (!ca) {
			ArchetypeData& data = m_archetypes.GetArchetypeData(archetype);
			ca = m_components.emplace(archetype, new ComponentArray(6291456, data));
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
