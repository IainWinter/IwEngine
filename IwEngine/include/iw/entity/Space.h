#pragma once

#include "IwEntity.h"
#include "ComponentManager.h"
#include "ArchetypeManager.h"
#include "EntityManager.h"
#include "View.h"

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
		iwu::ref<const Component> RegisterComponent() {
			return RegisterComponent(typeid(_c), sizeof(_c));
		}

		iwu::ref<const Component> RegisterComponent(
			std::type_index type,
			size_t size);

		//template<
		//	typename _c>
		//iwu::ref<Component> GetComponent() {
		//	return GetComponent(typeid(_c));
		//}

		//iwu::ref<Component> GetComponent();

		template<
			typename... _cs>
		ComponentQuery MakeQuery() {
			return ComponentQuery {
				{ RegisterComponent<_cs>()... }
			};
		}

		// Archetypes

		template<
			typename... _cs>
		iwu::ref<const Archetype2> CreateArchetype() {
			return CreateArchetype({ RegisterComponent<_cs>()... });
		}

		iwu::ref<const Archetype2> CreateArchetype(
			std::initializer_list<iwu::ref<const Component>> components);

		// Entities

		template<
			typename... _cs>
		iwu::ref<Entity2> CreateEntity() {
			return CreateEntity(CreateArchetype<_cs...>());
		}

		iwu::ref<Entity2> CreateEntity(
			iwu::ref<const Archetype2> archetype);

		bool DestroyEntity(
			iwu::ref<const Entity2> entity);

		// Querying

		template<
			typename... _cs>
		EntityComponentArray Query() {
			return Query(MakeQuery<_cs...>());
		}

		EntityComponentArray Query(
			const ComponentQuery& query);
	};
}
