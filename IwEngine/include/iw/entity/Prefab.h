#pragma once

#include "IwEntity.h"
#include "Component.h"
#include "iw/util/memory/linear_allocator.h"
#include <vector>

// Making a prefab system! Goal is to make it so you can spawn one of these once setup (components with default data)
// Making a prefab system! Goal is to make it so you can spawn one of these once setup (components with default data)
// Making a prefab system! Goal is to make it so you can spawn one of these once setup (components with default data)
// Making a prefab system! Goal is to make it so you can spawn one of these once setup (components with default data)
// Making a prefab system! Goal is to make it so you can spawn one of these once setup (components with default data)

namespace iw {
namespace ECS {
	struct Prefab {
	private:
		std::vector<iw::ref<Component>> m_components;
		std::vector<char*> m_componentData; // offsets to m_memory.memory();
		linear_allocator m_memory;

	public:
		IWENTITY_API
		Prefab();

		// Adds a component & data if given to the prefab
		IWENTITY_API
		void Add(
			iw::ref<Component> component,
			void* prefab = nullptr);

		// Sets a component & data if given to the prefab. If the component does not exist no action is taken & false is returnd. True otherwise
		IWENTITY_API
		bool Set(
			iw::ref<Component> component,
			void* prefab = nullptr);

		IWENTITY_API
		void Remove(
			iw::ref<Component> component);

		IWENTITY_API
		bool Has(
			iw::ref<Component> component);

		IWENTITY_API
		const std::vector<iw::ref<Component>>& Components() const;

		IWENTITY_API
		const std::vector<void*>& ComponentData() const;

		IWENTITY_API
		unsigned ComponentCount() const;
	};

}
	using namespace ECS;
}
