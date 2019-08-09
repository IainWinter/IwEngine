#pragma once

#include "iw/entity2/IwEntity.h"
#include <vector>
#include <type_traits>

namespace IwEntity2 {
	class IWENTITY2_API Archetype {
	private:
		std::vector<Component> m_components;
		std::vector<size_t>    m_sizes;

	public:
		bool operator==(
			const Archetype& other) const;

		void AddComponent(
			Component id,
			size_t size);

		void RmeoveComponent(
			Component id);

		size_t SizeBefore(
			Component id) const;

		size_t SizeAfter(
			Component id) const;

		const std::vector<Component>& Components() const {
			return m_components;
		}
	};
}

namespace std {
	template<>
	struct hash<
		IwEntity2::Archetype>
	{
		size_t operator()(
			const IwEntity2::Archetype& archetype) const noexcept
		{
			auto components = archetype.Components();
			size_t seed = components.size();
			for (IwEntity2::Component c : components) {
				seed ^= c + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}

			return seed;
		}
	};
}

