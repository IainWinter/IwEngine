#pragma once

#include "IwEntity.h"
#include "Component.h"
#include "iw/util/memory/linear_allocator.h"
#include <vector>

namespace iw {
namespace ECS {
	struct Prefab {
	private:
		std::vector<ref<Component>> m_components;
		std::vector<size_t> m_offsets; // offsets to m_memory.memory();
		linear_allocator m_memory;

	public:
		IWENTITY_API
		Prefab();

		~Prefab() {}

		// Adds a component & data if given to the prefab
		IWENTITY_API
		void Add(
			ref<Component> component,
			void* data = nullptr);

		// Sets a component & data if given to the prefab. If the component does not exist no action is taken & false is returnd. True otherwise
		IWENTITY_API
		bool Set(
			ref<Component> component,
			void* data = nullptr);

		IWENTITY_API
		void Remove(
			ref<Component> component);

		IWENTITY_API
		bool Has(
			ref<Component> component);

		IWENTITY_API
		const ref<Component>& GetComponent(
			size_t index) const;

		IWENTITY_API
		void* GetComponentData(
			size_t index) const;

		IWENTITY_API
		size_t ComponentCount() const;
	};

}
	using namespace ECS;
}
