#pragma once

#include "iw/entity/Space.h"
#include <assert.h>

namespace iw {
namespace ECS {
	ref<Component>& Space::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		return m_componentManager.RegisterComponent(type, size);
	}

	ref<Component> Space::GetComponent(
		ComponentType type)
	{
		return m_componentManager.GetComponent(type);
	}

	Archetype Space::CreateArchetype(
		std::initializer_list<ref<Component>> components)
	{
		return m_archetypeManager.CreateArchetype(components);
	}

	Entity Space::CreateEntity(
		const Archetype& archetype)
	{
		EntityData& entityData = m_entityManager.CreateEntity();

		bool componentsExist = false;
		if (   entityData.Archetype.Layout
			&& entityData.Archetype.Hash == archetype.Hash)
		{
			componentsExist = m_componentManager.ReinstateComponentData(entityData); // combine this logic into cmgr.CreateComponentData
		}

		else {
			entityData.Archetype = archetype;
		}

		if (!componentsExist) {
			entityData.ChunkIndex = m_componentManager.CreateComponentsData(entityData);
		}

		TrySendEntityCreatedEvent(entityData.Entity);

		return Entity(entityData.Entity, this);
	}

	bool Space::DestroyEntity(
		EntityHandle handle)
	{
		if (!m_entityManager.IsValidEntityIndex(handle.Index)) {
			LOG_WARNING << "Tried to destroy an entity with invalid index " << handle.Index;
			return false;
		}

		EntityData& entityData = m_entityManager.GetEntityData(handle.Index);

		TrySendEntityDestroiedEvent(entityData.Entity);

		return m_entityManager   .DestroyEntity(handle.Index)
			&& m_componentManager.DestroyComponentsData(entityData);
	}

	bool Space::KillEntity(
		EntityHandle handle)
	{
		if (!m_entityManager.IsValidEntityIndex(handle.Index)) {
			LOG_WARNING << "Tried to kill an entity with invalid index " << handle.Index;
			return false;
		}

		EntityData& entityData = m_entityManager.GetEntityData(handle.Index);
		entityData.Entity.Alive = false;

		return m_componentManager.SetEntityAliveState(entityData);
	}

	Entity Space::ReviveEntity(
		EntityHandle handle)
	{
		if (!m_entityManager.IsValidEntityIndex(handle.Index)) {
			LOG_WARNING << "Tried to revive an entity with invalid index " << handle.Index;
			return Entity();
		}

		EntityData& entityData = m_entityManager.GetEntityData(handle.Index);
		entityData.Entity.Alive = true;

		bool success = m_componentManager.SetEntityAliveState(entityData);
		if (!success) {
			return Entity(EntityHandle::Empty, this);
		}

		return Entity(entityData.Entity, this);
	}

	void Space::AddComponent(
		EntityHandle handle,
		const ref<Component>& component)
	{
		if (!m_entityManager.IsValidEntityIndex(handle.Index)) {
			LOG_WARNING << "Tried to add " << component->Name
				        << " component to an entity with invalid index " << handle.Index;
			return;
		}

		if (!component) {
			LOG_WARNING << "Tried to add empty component to entity " << handle.Index;
			return;
		}

		EntityData& entityData = m_entityManager   .GetEntityData(handle.Index);
		Archetype   archetype  = m_archetypeManager.AddComponent (entityData.Archetype, component);

		MoveComponents(entityData, archetype);
	}

	void Space::RemoveComponent(
		EntityHandle handle,
		const ref<Component>& component)
	{
		if (!m_entityManager.IsValidEntityIndex(handle.Index)) {
			LOG_WARNING << "Tried to remove a(n) " << component->Name
				        << " component from an entity with invalid index " << handle.Index;
			return;
		}

		if (!component) {
			LOG_WARNING << "Tried to remove empty component from entity " << handle.Index;
			return;
		}

		EntityData& entityData = m_entityManager   .GetEntityData  (handle.Index);
		Archetype   archetype  = m_archetypeManager.RemoveComponent(entityData.Archetype, component);

		MoveComponents(entityData, archetype);
	}

	void  Space::AddComponent(
		Archetype& archetype, 
		const ref<Component>& component)
	{
		if (!archetype.Layout) {
			LOG_WARNING << "Tried to add component to empty archetype " << component->Name;
			return;
		}

		if (!component) {
			LOG_WARNING << "Tried to add empty component to archetype " << archetype.Hash;
			return;
		}

		archetype = m_archetypeManager.AddComponent(archetype, component);
	}

	void Space::RemoveComponent(
		Archetype& archetype,
		const ref<Component>& component)
	{
		if (!archetype.Layout) {
			LOG_WARNING << "Tried to remove component from empty archetype " << component->Name;
			return;
		}

		if (!component) {
			LOG_WARNING << "Tried to remove empty component from archetype " << archetype.Hash;
			return;
		}

		archetype = m_archetypeManager.RemoveComponent(archetype, component);
	}

	void* Space::SetComponent(
		EntityHandle handle,
		const ref<Component>& component,
		void* data)
	{
		if (!m_entityManager.IsValidEntityIndex(handle.Index)) {
			LOG_WARNING << "Tried to set a(n) " << component->Name
				        << " component's data on an entity with invalid index " << handle.Index;
			return nullptr;
		}

		if (!component) {
			LOG_WARNING << "Tried to set empty component on entity " << handle.Index;
			return nullptr;
		}

		void* ptr = m_componentManager.GetComponentPtr(m_entityManager.GetEntityData(handle.Index), component);
		
		if (ptr && data) {
			component->DeepCopyFunc(ptr, data);
		}

		return ptr;
	}

	void* Space::FindComponent(
		EntityHandle handle,
		const ref<Component>& component)
	{
		if (!m_entityManager.IsValidEntityIndex(handle.Index)) {
			LOG_WARNING << "Tried to find a(n) " << component->Name
				        << " component on an entity with invalid index " << handle.Index;
			return nullptr;
		}

		if (!component) {
			LOG_WARNING << "Tried to find empty component on entity " << handle.Index;
			return nullptr;
		}

		EntityData& entityData = m_entityManager.GetEntityData(handle.Index);
		return m_componentManager.GetComponentPtr(entityData, component);
	}

	bool Space::HasComponent(
		EntityHandle handle,
		const ref<Component>& component)
	{
		if (!m_entityManager.IsValidEntityIndex(handle.Index)) {
			LOG_WARNING << "Tried to test if a(n) " << component->Name
				        << " component exists on an entity with invalid index " << handle.Index;
			return false;
		}

		if (!component) {
			//LOG_WARNING << "Tried to find empty component on entity " << handle.Index;
			return false;
		}

		EntityData& entityData = m_entityManager.GetEntityData(handle.Index);
		return entityData.Archetype.HasComponent(component);
	}

	Archetype Space::GetArchetype(
		EntityHandle handle)
	{
		if (!m_entityManager.IsValidEntityIndex(handle.Index)) {
			LOG_WARNING << "Tried to get the archetype of an entity with invalid index " << handle.Index;
			return Archetype();
		}

		EntityData& entityData = m_entityManager.GetEntityData(handle.Index);
		return entityData.Archetype;
	}

	ComponentQuery Space::MakeQuery(
		std::initializer_list<ref<Component>> components)
	{
		return m_componentManager.MakeQuery(components);
	}

	EntityComponentArray Space::Query(
		const ComponentQuery& query)
	{
		return m_componentManager.Query(query, m_archetypeManager.MakeQuery(query));
	}

	Entity Space::FindEntity(
		const ref<Component>& component,
		void* instance)
	{
		if (!component) {
			LOG_WARNING << "Tried to find an entity with an empty component";
			return Entity();
		}

		EntityHandle entity = m_componentManager.FindEntity(
			m_archetypeManager.MakeQuery(MakeQuery({ component })),
			component,
			instance);

		return Entity(entity, this);
	}

	Entity Space::GetEntity(
		EntityHandle handle)
	{
		return Entity(m_entityManager.GetEntityData(handle.Index).Entity, this);
	}

	Entity Space::Instantiate(
		const Prefab& prefab)
	{
		Archetype archetype = m_archetypeManager.CreateArchetype(
										&prefab.GetComponent(0),
										&prefab.GetComponent(prefab.ComponentCount() - 1) + 1);

		Entity entity = CreateEntity(archetype);

		for (unsigned i = 0; i < prefab.ComponentCount(); i++) {
			ref<Component> component = prefab.GetComponent(i);
			void*          data      = prefab.GetComponentData(i);

			SetComponent(entity.Handle, component, data);
		}

		return entity;
	}

	void Space::Clear() {
		for (const EntityData* data : m_entityManager.Entities()) {
			TrySendEntityDestroiedEvent(data->Entity);
		}

		m_entityManager.Clear();
		m_componentManager.Clear();
		m_archetypeManager.Clear();
	}

	std::vector<EntityHandle> Space::Entities() const {
		const std::vector<EntityData*>& data = m_entityManager.Entities();

		std::vector<EntityHandle> handles;
		handles.reserve(data.size());

		for (const EntityData* ed : data) {
			if (ed->Entity.Alive) {
				handles.emplace_back(ed->Entity);
			}
		}

		return handles;
	}

	const std::vector<Archetype>& Space::Archetypes() const {
		return m_archetypeManager.Archetypes();
	}

	void Space::ExecuteQueue() {
		while (!m_propQueue.empty()) {
			PropChange& change = m_propQueue.front();

			change.copy(change.prop, change.value);
			delete change.value;

			m_propQueue.pop();
		}

		while (!m_entityQueue.empty()) {
			EntityChange& change = m_entityQueue.front();

			if (m_entityManager.GetEntityData(change.handle.Index).Entity.Alive)
			{
				change.func(Entity(change.handle, this));
			}

			m_entityQueue.pop();
		}
	}

	void Space::QueueEntity(
		EntityHandle entity,
		func_EntityChange func)
	{
		m_entityQueue.push({
			entity,
			func
		});
	}

	void Space::QueueEntity(
		void* prop, 
		void* change, 
		func_DeepCopy& copy)
	{
		m_propQueue.push({
			prop,
			change,
			copy
		});
	}

	void Space::MoveComponents(
		EntityData& entityData,
		const Archetype& newArchetype)
	{
		TrySendEntityMovedEvent(entityData.Entity, entityData.Archetype, newArchetype);

		entityData.ChunkIndex = m_componentManager.MoveComponentData(entityData, newArchetype);
		entityData.Archetype = newArchetype;
	}
}
}
