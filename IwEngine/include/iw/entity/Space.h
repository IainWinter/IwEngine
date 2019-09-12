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
		template<
			typename _c>
		iwu::ref<const Component> RegisterComponent() {
			return RegisterComponent(typeid(_c), sizeof(_c));
		}

		template<
			typename... _cs>
		iwu::ref<const Archetype2> CreateArchetype() {
			return CreateArchetype({ RegisterComponent<_cs>()... });
		}

		template<
			typename... _cs>
		iwu::ref<IwEntity::Entity2> CreateEntity() {
			return CreateEntity(CreateArchetype<_cs...>());
		}

		iwu::ref<const Component> RegisterComponent(
			std::type_index type,
			size_t size);

		iwu::ref<const Archetype2> CreateArchetype(
			std::initializer_list<iwu::ref<const Component>> components);

		iwu::ref<Entity2> CreateEntity(
			iwu::ref<const Archetype2> archetype);

		bool DestroyEntity(
			iwu::ref<const Entity2> entity);
	};
}
