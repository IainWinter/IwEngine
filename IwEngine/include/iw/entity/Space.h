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
		std::weak_ptr<Component> RegisterComponent() {
			return RegisterComponent(typeid(_c), sizeof(_c));
		}

		template<
			typename... _cs>
		std::weak_ptr<Archetype2> CreateArchetype() {
			return CreateArchetype({ RegisterComponent<_cs>()... });
		}

		template<
			typename... _cs>
		size_t CreateEntity() {
			return CreateEntity(CreateArchetype<_cs...>());
		}

		std::weak_ptr<Component> RegisterComponent(
			std::type_index type,
			size_t size);

		std::weak_ptr<Archetype2> CreateArchetype(
			std::initializer_list<std::weak_ptr<Component>> components);

		size_t CreateEntity(
			std::weak_ptr<Archetype2> archetype);

		bool DestroyEntity(
			size_t entityIndex);
	};
}
