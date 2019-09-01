#pragma once

#include "IwEntity.h"
#include "Component.h"
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace IwEntity {
	class ComponentManager {
	private:
		std::unordered_map<std::type_index, std::shared_ptr<Component>> m_components;
		
	public:
		template<
			typename _c>
		std::weak_ptr<Component> GetComponent() {
			std::type_index info = typeid(_c);

			auto component = m_components[info];
			if (!component) {
				component = std::make_shared<Component>();
				component->Type = info.hash_code();
				component->Name = info.name();
				component->Size = sizeof(_c);
			}

			return component;
		}
	};
}
