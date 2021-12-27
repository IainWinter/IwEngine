#pragma once

#include "IwEntity.h"
#include "ComponentManager.h"
#include "ArchetypeManager.h"
#include "EntityManager.h"
#include "Prefab.h"
#include "iw/log/logger.h"
#include <queue>

#ifdef IW_USE_EVENTS
#	include "iw/events/eventbus.h"
#	include "iw/entity/Events/EntityEvents.h"
#endif

namespace iw {
namespace ECS {
	struct REFLECT Entity;
	using func_EntityChange = std::function<void(Entity)>;

	class Space {
	private:
		ComponentManager m_componentManager;
		ArchetypeManager m_archetypeManager;
		EntityManager    m_entityManager;

#ifdef IW_USE_EVENTS
		ref<eventbus> m_bus;
#endif

		// idk if this should be here

		struct PropChange { // only supports integral types
			void* prop;
			void* value;
			func_DeepCopy copy;
		};

		struct EntityChange {
			EntityHandle handle;
			func_EntityChange func;
		};

		linear_allocator m_propMem;

		// queue of stuff
		std::queue<PropChange>   m_propQueue;
		std::queue<EntityChange> m_entityQueue;

	public:
		// Registers a component type with the component manager allowing it to be used in archetypes
		IWENTITY_API
		ref<Component>& RegisterComponent(
			ComponentType type,
			size_t size);

		// Gets a registed component from the component manager if one exists
		IWENTITY_API
		ref<Component> GetComponent(
			ComponentType type);

		// Creates an archetype from a list of registed components
		IWENTITY_API
		Archetype CreateArchetype(
			std::initializer_list<ref<Component>> components);

		// Recycles or creates a new entity with components of the specified archetype
		IWENTITY_API
		Entity CreateEntity(
			const Archetype& archetype);

		// Destroys an entity and its components
		IWENTITY_API
		bool DestroyEntity(
			EntityHandle handle);

		// Marks an entity as dead, gets skipped by iterator, doesn't touch components
		IWENTITY_API
		bool KillEntity(
			EntityHandle handle);

		// Marks an entity as alive, doesn't touch components
		IWENTITY_API
		Entity ReviveEntity(
			EntityHandle handle);

		// Adds a new component to an entity
		IWENTITY_API
		void AddComponent(
			EntityHandle handle,
			const ref<Component>& component);

		// Removes a component from an entity
		IWENTITY_API
		void RemoveComponent(
			EntityHandle handle,
			const ref<Component>& component);
		
		// Adds a new component to an archetype
		IWENTITY_API
		void AddComponent(
			Archetype& archetype,
			const ref<Component>& component);

		// Removes a component from an archetype
		IWENTITY_API
		void RemoveComponent(
			Archetype& archetype,
			const ref<Component>& component);

		// Sets component data with a copy
		IWENTITY_API
		void* SetComponent(
			EntityHandle handle,
			const ref<Component>& component,
			void* data);

		// Finds a pointer to the component data of an entity
		IWENTITY_API
		void* FindComponent(
			EntityHandle handle,
			const ref<Component>& component);

		// Returns true if the entities archetype contains the component
		IWENTITY_API
		bool HasComponent(
			EntityHandle handle,
			const ref<Component>& component);

		IWENTITY_API
		Archetype GetArchetype(
			EntityHandle handle);

		// Makes a component query from a list of registered components
		IWENTITY_API
		ComponentQuery MakeQuery(
			std::initializer_list<ref<Component>> components);

		// Query for entities with the specified components
		IWENTITY_API
		EntityComponentArray Query(
			const ComponentQuery& query);

		// Finds an entity from one of its components
		IWENTITY_API
		Entity FindEntity(
			const ref<Component>& component,
			void* instance);

		// Gets an entity from a handle's index
		IWENTITY_API
		Entity GetEntity(
			EntityHandle handle);

		// Gets an entity from an index
		IWENTITY_API
		Entity GetEntity(
			size_t index);

		IWENTITY_API
		Entity Instantiate(
			const Prefab& prefab);

// ---------------------------------------------------------
//
// Templated versions of above functions
//
// ---------------------------------------------------------

		// Registers a component type with the component manager allowing it to be used in archetypes
		template<
			typename _c>
		ref<Component>& RegisterComponent() {
			ref<Component>& component = RegisterComponent(
#ifdef IW_USE_REFLECTION
				GetType<_c>(), GetType<_c>()->size
#else
				typeid(_c), sizeof(_c)
#endif
			);

			component->DeepCopyFunc   = GetCopyFunc<_c>();
			component->DestructorFunc = GetDestructorFunc<_c>();

			return component;
		}

		// Gets a registed component from the component manager if one exists
		template<
			typename _c>
		ref<Component> GetComponent() {
			return GetComponent(
#ifdef IW_USE_REFLECTION
				GetType<_c>()
#else
				typeid(_c)
#endif
			);
		}

		// Creates an archetype from a list of registed components
		template<
			typename... _cs>
		Archetype CreateArchetype() {
			return CreateArchetype({ RegisterComponent<_cs>()... });
		}

		// Recycles or creates a new entity with components of the specified archetype
		template<
			typename... _cs>
		Entity CreateEntity() {
			//Entity entity = CreateEntity(CreateArchetype<_cs...>());
			//std::tuple(SetComponent<_cs>()...);
			return CreateEntity(CreateArchetype<_cs...>());
		}

		// Adds a component to an entity and constructs it inplace
		template<
			typename _c,
			typename... _args>
		_c* AddComponent(
			EntityHandle handle,
			_args&&... args)
		{
			AddComponent(handle, RegisterComponent<_c>());
			return SetComponent<_c>(handle, args...);
		}

		// Removes a component from an entity
		template<
			typename _c>
		void RemoveComponent(
			EntityHandle handle)
		{
			RemoveComponent(handle, GetComponent<_c>());
		}

		// Adds a component to an entity and constructs it inplace
		template<
			typename _c>
		void AddComponent(
			Archetype& archetype)
		{
			AddComponent(archetype, RegisterComponent<_c>());
		}

		// Removes a component from an entity
		template<
			typename _c>
		void RemoveComponent(
			Archetype& archetype)
		{
			RemoveComponent(archetype, GetComponent<_c>());
		}

		// Sets component data by constructing it inplace
		template<
			typename _c,
			typename... _args>
		_c* SetComponent(
			EntityHandle handle,
			_args&&... args)
		{
			EntityData& entityData = m_entityManager.GetEntityData(handle.Index);
			ref<Component>   component  = GetComponent<_c>();

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
			EntityHandle handle)
		{
			if (ref<Component> component = GetComponent<_c>())
			{
				return (_c*)FindComponent(handle, component);
			}

			return nullptr;
		}

		// Returns true if the entities archetype contains the component
		template<
			typename _c>
		bool HasComponent(
			EntityHandle handle)
		{
			if (ref<Component> component = GetComponent<_c>())
			{
				return (_c*)HasComponent(handle, component);
			}

			return false;
		}

		// Makes a component query from a list of registered components
		template<
			typename... _cs>
		ComponentQuery MakeQuery() {
			return MakeQuery({ RegisterComponent<_cs>()... });
		}

		// Query for entities with the specified components
		template<
			typename... _cs>
		ECA<_cs...> Query() {
			return Query(MakeQuery<_cs...>());
		}

		// Finds an entity from one of its components. Allows for instance to be inherited
		template<
			typename _c>
		Entity FindEntity(
			void* instance)
		{
			if (ref<Component> component = GetComponent<_c>())
			{
				return FindEntity(component, instance);
			}

			return Entity();
		}

		// Finds an entity from one of its components
		template<
			typename _c>
		Entity FindEntity(
			_c* instance)
		{
			if (ref<Component> component = GetComponent<_c>())
			{
				return FindEntity(component, (void*)instance);
			}

			return Entity();
		}

// ---------------------------------------------------------
//
// 'Helper' functions
//
// ---------------------------------------------------------

		// Clears space of entities and all component data
		IWENTITY_API
		void Clear();

		IWENTITY_API
		std::vector<EntityHandle> Entities() const;

		IWENTITY_API
		const std::vector<Archetype>& Archetypes() const;

#ifdef IW_USE_EVENTS
		void SetEventBus(
			ref<eventbus> bus)
		{
			m_bus = bus;
		}
#endif
// ---------------------------------------------------------
//
// 'Post frame' functions for queueing (might move to another class)
//
// ---------------------------------------------------------

		IWENTITY_API
		void ExecuteQueue();

		template<
			typename _t>
		void QueueChange(
			_t* prop,
			_t  value)
		{
			static_assert(std::is_integral_v<_t>); // make sure isn't a vector or something

			QueueEntity(
				prop,
				new _t(value),
				GetCopyFunc<_t>()
			);
		}

		IWENTITY_API
		void QueueEntity(
			EntityHandle entity,
			func_EntityChange func);
	private:
		IWENTITY_API
		void QueueEntity(
			void* prop,
			void* change,
			func_DeepCopy& copy);

		// Moves components from one chunk list to another
		void MoveComponents(
			EntityData& entityData,
			const Archetype& newArchetype);

		// sending events allows disabling by user

		// theses dont get disabled if define is undefed?????

		void TrySendEntityCreatedEvent(
			const EntityHandle& handle)
		{
#ifdef IW_USE_EVENTS
			if (m_bus) {
				m_bus->send<EntityCreatedEvent>(handle);
			}
#endif
		}

		void TrySendEntityDestroiedEvent(
			const EntityHandle& handle)
		{
#ifdef IW_USE_EVENTS
			if (m_bus) {
				m_bus->send<EntityDestroyedEvent>(handle);
			}
#endif
		}

		void TrySendEntityMovedEvent(
			const EntityHandle& handle,
			const Archetype& oldArchetype,
			const Archetype& newArchetype)
		{
#ifdef IW_USE_EVENTS
			if (m_bus) { // this needs two events maybe to capture all the info from before and after to
						 // do anything useful with it
				m_bus->send<EntityMovedEvent>(handle, oldArchetype, newArchetype);
			}
#endif
		}
	};
}

	using namespace ECS;
}
