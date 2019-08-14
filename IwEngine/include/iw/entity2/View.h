#pragma once

#include "IwEntity.h"
#include "ComponentArray.h"
#include "iw/util/memory/smart_pointers.h"

namespace IwEntity2 {
	class IWENTITY2_API View {
	public:
		class Iterator;

		class ComponentData {
		private:
			void*                m_components;
			const ArchetypeData* m_archetype;

		public:
			const IwEntity2::Entity Entity;

		private:
			friend class Iterator;

			ComponentData(
				IwEntity2::Entity entity,
				void* components,
				const ArchetypeData* archetypes)
				: Entity(entity)
				, m_components(components)
				, m_archetype(archetypes)
			{}
		public:
			ComponentData() = delete;
			ComponentData(ComponentData&&) = delete;
			ComponentData(ComponentData&)  = delete;

			bool operator!=(
				const ComponentData& other) const
			{
				return Entity != other.Entity;
			}

			template<
				typename _s>
			_s& GetComponents() {
				m_archetype.size
				return *(_s*)m_components;
			}
		};

		class Iterator {
		private:
			using CIterator = ComponentArray::Iterator;

			size_t m_current;
			std::vector<CIterator> m_itrs;

		public:
			Iterator(
				std::vector<CIterator>&& itrs);

			Iterator& operator++() {
				++m_itrs[m_current];

				return *this;
			}

			bool operator!=(
				const Iterator& other) const
			{
				return m_current == other.m_current
				    && m_itrs != other.m_itrs;
			}

			ComponentData operator*() {
				CIterator& itr = m_itrs[m_current];
				return ComponentData(itr.entity(), *itr, &itr.Archetype());
			}
		};
	private:
		const std::vector<ComponentArray*> m_componentArrays;

	public:
		View(
			std::vector<ComponentArray*>&& componentArrays);

		Iterator begin();
		Iterator end();
	};
}
