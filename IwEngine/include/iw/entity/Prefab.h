#pragma once

#include "IwEntity.h"
#include "Component.h"
#include "iw/util/memory/linear_allocator.h"
#include <vector>

namespace iw {
namespace ECS {
	struct Prefab {
	private:
		std::vector<iw::ref<Component>> m_components;
		std::vector<size_t> m_offsets; // offsets to m_memory.memory();
		linear_allocator m_memory;

	public:
		IWENTITY_API
		Prefab();

		~Prefab() {}

		// Adds a component & data if given to the prefab
		IWENTITY_API
		void Add(
			iw::ref<Component> component,
			void* data = nullptr);

		// Sets a component & data if given to the prefab. If the component does not exist no action is taken & false is returnd. True otherwise
		IWENTITY_API
		bool Set(
			iw::ref<Component> component,
			void* data = nullptr);

		IWENTITY_API
		void Remove(
			iw::ref<Component> component);

		IWENTITY_API
		bool Has(
			iw::ref<Component> component);

		IWENTITY_API
		const iw::ref<Component>& GetComponent(
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
