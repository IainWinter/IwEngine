#pragma once

#include "iw/physics/AABB.h"
#include <vector>
#include <unordered_map>

namespace iw {
namespace Physics {
	template<
		typename T,
		typename V = glm::vec3,
		typename K = int>
	class Grid {
	private:
		using AABB = impl::AABB<V>;

		struct Cell {
			std::vector<T> Items;
		};

		V m_cellDimensions;
		std::unordered_map<K, Cell> m_cells;

	public:
		Grid(
			V cellDimensions)
			: m_cellDimensions(cellDimensions)
		{}

		void Insert(
			const T& item,
			const V& position,
			const AABB& bounds)
		{
			K key = GetKey<V>(position, bounds);
			auto itr = m_cells.find(key);
			if (itr == m_cells.end()) {
				itr = m_cells.insert(std::make_pair(key, Cell())).first;
			}

			Cell& cell = itr->second;
			cell.Items.push_back(item);
		}

		size_t CellCount() {
			return m_cells.size();
		}
	private:
		template<
			typename D>
		K GetKey(
			const V& position,
			const AABB& bounds);

		template<>
		K GetKey<glm::vec2>(
			const V& position,
			const AABB& bounds)
		{
			glm::vec3 key = (bounds.Center() + position) / m_cellDimensions;
			K x = (K)key.x;
			K y = (K)key.y;

			constexpr static size_t KEYPART = sizeof(K) * 4 / 2;

			return x << (KEYPART * 0)
				+ y << (KEYPART * 1);
		}

		template<>
		K GetKey<glm::vec3>(
			const V& position,
			const AABB& bounds)
		{
			glm::vec3 key = (bounds.Center() + position) / m_cellDimensions;
			K x = (K)key.x;
			K y = (K)key.y;
			K z = (K)key.z;

			constexpr static size_t KEYPART = sizeof(K) * 4 / 3;

			return x << (KEYPART * 0)
				+ y << (KEYPART * 1)
				+ z << (KEYPART * 2);
		}

		template<>
		K GetKey<glm::vec4>(
			const V& position,
			const AABB& bounds)
		{
			glm::vec3 key = (bounds.Center() + position) / m_cellDimensions;
			K x = (K)key.x;
			K y = (K)key.y;
			K z = (K)key.z;
			K w = (K)key.w;

			constexpr static size_t KEYPART = sizeof(K) * 4 / 4;

			return x << (KEYPART * 0)
				+ y << (KEYPART * 1)
				+ z << (KEYPART * 2)
				+ z << (KEYPART * 3);
		}
	};
}

	using namespace Physics;
}
