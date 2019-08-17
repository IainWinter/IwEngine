#include "iw/entity2/Space.h"

namespace IwEntity2 {
	EntityIndex Space::CreateEntity() {
		return m_entities.CreateEntity();
	}

	bool Space::DestroyEntity(
		EntityIndex entity)
	{
		Entity& data = m_entities.GetEntityData(entity);
		if (data.Alive) {
			ComponentManager* ca = GetComponentArray(data.Archetype);
			ca->DestroyComponents(entity);
		}

		return m_entities.DestroyEntity(entity);
	}

	bool Space::EntityExists(
		EntityIndex entity)
	{
		return m_entities.EntityExists(entity);
	}

	void* Space::CreateComponent(
		EntityIndex entity,
		size_t componentSize,
		ComponentType componentType)
	{
		if (!EntityExists(entity)) {
			return nullptr;
		}

		EntityArchetype& newEntityArchetype = m_entities.GetEntityData(entity).Archetype;
		EntityArchetype  oldEntityArchetype = newEntityArchetype;

		m_archetypes.AddComponent(newEntityArchetype, componentType, componentSize);

		ComponentManager* oldca = GetComponentArray(oldEntityArchetype);
		ComponentManager& newca = EnsureComponentArray(newEntityArchetype);
		
		void* components = newca.CreateComponents(entity);

		if (oldca) {
			size_t olds  = oldca->Archetype().Size();
			size_t newsb = newca .Archetype().SizeBefore(componentType);
			size_t newsa = newca .Archetype().SizeAfter(componentType);

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

			// New component is in the midst
			else {
				memmove(newcomponents, oldcomponents, newsb);
				memmove(newcomponents + newsb + componentSize, oldcomponents + newsb, newsa);
			}

			oldca->DestroyComponents(entity);

			return newcomponents + newsb;
		}

		return components;
	}

	bool Space::DestroyComponent(
		EntityIndex entity,
		ComponentType componentType)
	{
		if (!EntityExists(entity)) {
			return false;
		}

		EntityArchetype& entityArchetype = m_entities.GetEntityData(entity).Archetype;

		ComponentManager* oldca = GetComponentArray(entityArchetype);

		m_archetypes.RemoveComponent(entityArchetype, componentType);

		if (entityArchetype != 0) {
			ComponentManager& newca = EnsureComponentArray(entityArchetype);

			size_t s  = oldca->Archetype().Size();
			size_t sb = oldca->Archetype().SizeBefore(componentType);
			size_t sa = oldca->Archetype().SizeAfter(componentType);
			size_t cs = oldca->Archetype().ComponentSize(componentType);

			char* newcomponents = (char*)newca.CreateComponents(entity);
			char* oldcomponents = (char*)oldca->GetComponents(entity);

			// Can always treat as being in the middle
			memmove(newcomponents, oldcomponents, sb);
			memmove(newcomponents + sb, oldcomponents + sb + cs, sa);
		}

		oldca->DestroyComponents(entity);

		return true;
	}

	void* Space::GetComponent(
		EntityIndex entity,
		ComponentType componentType)
	{
		EntityArchetype entityArchetype = m_entities.GetEntityData(entity).Archetype;
		ComponentManager* componentArray  = GetComponentArray(entityArchetype);
		
		void*  components = componentArray->GetComponents(entity);
		size_t before     = componentArray->Archetype().SizeBefore(componentType);

		return (char*)components + before;
	}

	View Space::ViewComponents(
		const Archetype& query)
	{
		std::vector<EntityArchetype> archetypes = m_archetypes.Query(query);

		std::vector<ComponentManager*> cas;
		for (int i = 0; i < archetypes.size(); i++) {
			cas.push_back(GetComponentArray(archetypes.at(i)));
		}

		return View(std::move(cas));
	}

	ComponentManager* Space::GetComponentArray(
		EntityArchetype entityArchetype)
	{
		if (entityArchetype - 1 < m_components.size()) {
			return m_components[entityArchetype - 1];
		}

		return nullptr;
	}

	ComponentManager& Space::EnsureComponentArray(
		EntityArchetype entityArchetype)
	{
		ComponentManager* ca = GetComponentArray(entityArchetype);
		if (ca == nullptr) {
			Archetype& archetype = m_archetypes.GetArchetype(entityArchetype);
			
			ca = new ComponentManager(16384, archetype);
			m_components.emplace(m_components.begin() + (entityArchetype - 1), ca);
		}

		return *ca;
	}

	ComponentType Space::GetComponentId(
		std::type_index&& type)
	{
		static int nextId = 0;

		auto itr = m_componentTypes.find(type);
		if (itr == m_componentTypes.end()) {
			itr = m_componentTypes.emplace(type, nextId++).first;
		}

		return itr->second;
	}
}
