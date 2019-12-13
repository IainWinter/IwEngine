#pragma once

#include "IwEntity.h"
#include "ComponentManager.h"
#include "ArchetypeManager.h"
#include "EntityManager.h"
#include "iw/log/logger.h"

namespace IW {
namespace ECS {
	class Space {
	private:
		ComponentManager m_componentManager;
		ArchetypeManager m_archetypeManager;
		EntityManager    m_entityManager;

	public:
		// Components

		template<
			typename _c>
		iw::ref<Component>& RegisterComponent() {
			return RegisterComponent(typeid(_c), sizeof(_c));
		}

		IWENTITY_API
		iw::ref<Component>& RegisterComponent(
			ComponentType type,
			size_t size);

		template<
			typename _c>
		iw::ref<Component> GetComponent() {
			return GetComponent(typeid(_c));
		}

		IWENTITY_API
		iw::ref<Component> GetComponent(
			ComponentType type);

		template<
			typename... _cs>
		iw::ref<ComponentQuery> MakeQuery() {
			return MakeQuery({ RegisterComponent<_cs>()... });
		}

		IWENTITY_API
		iw::ref<ComponentQuery> MakeQuery(
			std::initializer_list<iw::ref<Component>> components);

		// Archetypes

		template<
			typename... _cs>
		iw::ref<Archetype>& CreateArchetype() {
			return CreateArchetype({ RegisterComponent<_cs>()... });
		}

		IWENTITY_API
		iw::ref<Archetype>& CreateArchetype(
			std::initializer_list<iw::ref<Component>> components);

		// Entities

		template<
			typename... _cs>
		Entity CreateEntity() {
			return CreateEntity(CreateArchetype<_cs...>());
		}

		IWENTITY_API
		Entity CreateEntity(
			const iw::ref<Archetype>& archetype);

		IWENTITY_API
		bool DestroyEntity(
			size_t index); // don't like this but idk how to delete from the iteration if not doing it like this

		template<
			typename _c,
			typename... _args>
		_c* SetComponentData(
			const Entity& entity,
			_args&&... args)
		{
			iw::ref<EntityData>& entityData = m_entityManager.GetEntityData(entity.Index);
			iw::ref<Component>   component  = m_componentManager.GetComponent(typeid(_c));

			if (component) {
				_c* data = (_c*)m_componentManager.GetComponentData(entityData, component);
				if (data) {
					if constexpr (std::is_aggregate_v<_c>) {
						*data = _c { std::forward<_args>(args)... };
					}

					else {
						new (data) _c(std::forward<_args>(args)...);
					}

					return data;
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

			return nullptr;
		}

		// Querying

		template<
			typename... _cs>
		EntityComponentArray Query() {
			return Query(MakeQuery<_cs...>());
		}

		IWENTITY_API
		EntityComponentArray Query(
			const iw::ref<ComponentQuery>& query);
	};
}

	using namespace ECS;
}
