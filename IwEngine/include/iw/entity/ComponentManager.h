#pragma once

#include "IwEntity.h"
#include "Component.h"
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace IwEntity {
	class ComponentManager {
	private:
		std::unordered_map<ComponentType, std::shared_ptr<Component>> m_components;
		
	public:
		std::weak_ptr<Component> RegisterComponent(
			ComponentType type,
			size_t size)
		{
			auto component = m_components[type];
			if (!component) {
				component = std::make_shared<Component>();
				component->Type = type.hash_code();
				component->Name = type.name();
				component->Size = size;
			}

			return component;
		}
	};
}
