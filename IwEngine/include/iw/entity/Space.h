#pragma once

#include "IwEntity.h"
#include "ComponentManager.h"
#include "ArchetypeManager.h"
#include "EntityManager.h"
#include "iw/log/logger.h"

namespace IW {
namespace ECS {
	struct Entity;

	class Space {
	private:
		ComponentManager m_componentManager;
		ArchetypeManager m_archetypeManager;
		EntityManager    m_entityManager;

	public:
		// Registers a component type with the component manager allowing it to be used in archetypes
		IWENTITY_API
		iw::ref<Component>& RegisterComponent(
			ComponentType type,
			size_t size);

		// Gets a registed component from the component manager if one exists
		IWENTITY_API
		iw::ref<Component> GetComponent(
			ComponentType type);

		// Creates an archetype from a list of registed components
		IWENTITY_API
		iw::ref<Archetype>& CreateArchetype(
			std::initializer_list<iw::ref<Component>> components);

		// Recycles or creates a new entity with components of the specified archetype
		IWENTITY_API
		Entity CreateEntity(
			const iw::ref<Archetype>& archetype);

		// Destroys an entity and its components
		IWENTITY_API
		bool DestroyEntity(
			size_t index);

		// Adds a new component to an entity
		IWENTITY_API
		void AddComponent(
			const EntityHandle& entity,
			const iw::ref<Component>& component);

		// Removes a component from an entity
		IWENTITY_API
		void RemoveComponent(
			const EntityHandle& entity,
			const iw::ref<Component>& component);

		// Sets component data with a copy
		IWENTITY_API
		void* SetComponent(
			const EntityHandle& entity,
			const iw::ref<Component>& component,
			void* data);

		// Finds a pointer to the component data of an entity
		IWENTITY_API
		void* FindComponent(
			const EntityHandle& entity,
			const iw::ref<Component>& component);

		// Returns true if the entities archetype contains the component
		IWENTITY_API
		bool HasComponent(
			const EntityHandle& entity,
			const iw::ref<Component>& component);

		// Makes a component query from a list of registered components
		IWENTITY_API
		iw::ref<ComponentQuery> MakeQuery(
			std::initializer_list<iw::ref<Component>> components);

		// Query for entities with the specified components
		IWENTITY_API
		EntityComponentArray Query(
			const iw::ref<ComponentQuery>& query);

		// Finds an entity from one of its components
		IWENTITY_API
		Entity FindEntity(
			const iw::ref<Component>& component,
			void* instance);

		// Registers a component type with the component manager allowing it to be used in archetypes
		template<
			typename _c>
		iw::ref<Component>& RegisterComponent() {
			return RegisterComponent(typeid(_c), sizeof(_c));
		}

		// Gets a registed component from the component manager if one exists
		template<
			typename _c>
		iw::ref<Component> GetComponent() {
			return GetComponent(typeid(_c));
		}

		// Creates an archetype from a list of registed components
		template<
			typename... _cs>
		iw::ref<Archetype>& CreateArchetype() {
			return CreateArchetype({ RegisterComponent<_cs>()... });
		}

		// Recycles or creates a new entity with components of the specified archetype
		template<
			typename... _cs>
		Entity CreateEntity() {
			return CreateEntity(CreateArchetype<_cs...>());
		}

		// Adds a component to an entity and constructs it inplace
		template<
			typename _c,
			typename... _args>
		_c* AddComponent(
			const EntityHandle& entity,
			_args&&... args)
		{
			AddComponent(entity, RegisterComponent<_c>());
			return SetComponent<_c>(entity, args...);
		}

		// Removes a component from an entity
		template<
			typename _c>
		void RemoveComponent(
			const EntityHandle& entity)
		{
			RemoveComponent(entity, GetComponent<_c>());
		}

		// Sets component data by constructing it inplace
		template<
			typename _c,
			typename... _args>
		_c* SetComponent(
			const EntityHandle& entity,
			_args&&... args)
		{
			iw::ref<EntityData>& entityData = m_entityManager   .GetEntityData(entity.Index);
			iw::ref<Component>   component  = m_componentManager.GetComponent (typeid(_c));

			if (component) {
				_c* ptr = (_c*)m_componentManager.GetComponentPtr(entityData, component);
				if (ptr) {
					if constexpr (std::is_aggregate_v<_c>) {
						*ptr = _c { std::forward<_args>(args)... };
					}

					else {
						new (ptr) _c(std::forward<_args>(args)...);
					}

					return ptr;
				}
			}

			return nullptr;
		}

		// Finds a pointer to the component data of an entity
		template<
			typename _c>
		_c* FindComponent(
			const EntityHandle& entity)
		{
			return (_c*)FindComponent(entity, GetComponent<_c>());
		}

		// Returns true if the entities archetype contains the component
		template<
			typename _c>
		bool HasComponent(
			const EntityHandle& entity)
		{
			return HasComponent(entity, GetComponent<_c>());
		}

		// Makes a component query from a list of registered components
		template<
			typename... _cs>
		iw::ref<ComponentQuery> MakeQuery() {
			return MakeQuery({ RegisterComponent<_cs>()... });
		}

		// Query for entities with the specified components
		template<
			typename... _cs>
		EntityComponentArray Query() {
			return Query(MakeQuery<_cs...>());
		}


		// Finds an entity from one of its components
		template<
			typename _c>
		Entity FindEntity(
			_c* instance) 		// returns invalid entity sometimes
		{
			return FindEntity(GetComponent<_c>(), instance);
		}
	private:
		// Moves components from one chunk list to another
		void MoveComponents(
			iw::ref<EntityData>& entityData,
			const iw::ref<Archetype>& archetype);
	};
}

	using namespace ECS;
}

#include "Entity.h" // ew but how do you avoid this
