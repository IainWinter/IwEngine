#pragma once

#include "IwEntity.h"
#include "Chunk.h"
#include "Entity.h"
#include "Component.h"
#include "ComponentData.h"
#include <unordered_map>
#include <memory>

namespace IwEntity {
	class IWENTITY_API ComponentManager {
	private:
		std::unordered_map<ComponentType, std::shared_ptr<Component>> m_components;
		std::unordered_map<size_t, Chunk*> m_componentData;

	public:
		std::weak_ptr<Component> RegisterComponent(
			ComponentType type,
			size_t size);

		std::shared_ptr<ComponentData> CreateComponents(
			Entity2& entity);

		bool DestroyComponents(
			Entity2& entity);
	};
}
