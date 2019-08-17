#pragma once

#include "iw/entity2/IwEntity.h"
#include "iw/util/set/sparse_set.h"

namespace IwEntity2 {
	struct ComponentTypeInfo {
		ComponentType Type;
		size_t        Size;
	};

	// Todo: Make this an struct with an array and more info
	class IWENTITY2_API Archetype {
	private:
		std::vector<ComponentTypeInfo> m_layout;

	public:
		bool operator==(
			const Archetype& other) const;

		bool EqualWith(
			Archetype& other,
			ComponentType id) const;

		bool EqualWithout(
			Archetype& other,
			ComponentType id) const;

		void AddComponent(
			ComponentType id,
			size_t size);

		void RemoveComponent(
			ComponentType id);

		bool HasComponent(
			ComponentType id) const;

		size_t ComponentSize(
			ComponentType id) const;

		size_t SizeBefore(
			ComponentType id) const;

		size_t SizeAfter(
			ComponentType id) const;

		size_t Size()  const;
		size_t Count() const;
		bool   Empty() const;
		void   Reset();

		inline const std::vector<ComponentTypeInfo>& Layout() const {
			return m_layout;
		}
	};
}
