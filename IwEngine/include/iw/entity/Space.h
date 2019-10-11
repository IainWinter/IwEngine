#pragma once

#include "IwEntity.h"
#include "ComponentManager.h"
#include "ArchetypeManager.h"
#include "EntityManager.h"
#include "iw/log/logger.h"

namespace IwEntity {
	class IWENTITY_API Space {
	private:
		ComponentManager m_componentManager;
		ArchetypeManager m_archetypeManager;
		EntityManager    m_entityManager;

	public:
		// Components

		template<
			typename _c>
		iwu::ref<Component>& RegisterComponent() {
			return RegisterComponent(typeid(_c), sizeof(_c));
		}

		iwu::ref<Component>& RegisterComponent(
			ComponentType type,
			size_t size);

		template<
			typename _c>
		iwu::ref<Component>& GetComponent() {
			return GetComponent(typeid(_c));
		}

		iwu::ref<Component>& GetComponent(
			ComponentType type);

		template<
			typename... _cs>
		iwu::ref<ComponentQuery> MakeQuery() {
			return MakeQuery({ RegisterComponent<_cs>()... });
		}

		iwu::ref<ComponentQuery> MakeQuery(
			std::initializer_list<iwu::ref<Component>> components);

		// Archetypes

		template<
			typename... _cs>
		iwu::ref<Archetype>& CreateArchetype() {
			return CreateArchetype({ RegisterComponent<_cs>()... });
		}

		iwu::ref<Archetype>& CreateArchetype(
			std::initializer_list<iwu::ref<Component>> components);

		// Entities

		template<
			typename... _cs>
		Entity CreateEntity() {
			return CreateEntity(CreateArchetype<_cs...>());
		}

		Entity CreateEntity(
			const iwu::ref<Archetype>& archetype);

		bool DestroyEntity(
			size_t index); // don't like this but idk how to delete from the iteration if not doing it like this

		template<
			typename _c,
			typename... _args>
		void SetComponentData(
			const Entity& entity,
			_args&&... args)
		{
			iwu::ref<EntityData>& entityData = m_entityManager.GetEntityData(entity.Index);
			iwu::ref<Component>&  component  = m_componentManager.GetComponent(typeid(_c));

			if (component) {
				_c* data = (_c*)m_componentManager.GetComponentData(entityData, component);
				if (data) {
					*data = _c{ std::forward<_args>(args)... };
				}

				else {
					LOG_WARNING << "Tried to set component data of non-attached component! Entity: "
						<< entity.Index << " Version: " << entity.Version;
				}
			}

			else {
				LOG_WARNING << "Tried to set component data of non-registerd component! Entity: "
					<< entity.Index << " Version: " << entity.Version;
			}
		}

		// Querying

		template<
			typename... _cs>
		EntityComponentArray Query() {
			return Query(MakeQuery<_cs...>());
		}

		EntityComponentArray Query(
			const iwu::ref<ComponentQuery>& query);
	};
}
