#pragma once

#include "iw/physics/AABB.h"
#include <vector>
#include <unordered_map>

namespace IwPhysics {
	template<
		typename T,
		typename Dim = iwm::vector3,
		typename Key = int>
	class Grid {
	private:
		using AABB = AABB<Dim>;

		struct Cell {
			std::vector<T> Items;
		};

		Dim m_cellDimensions;
		std::unordered_map<Key, Cell> m_cells;

	public:
		Grid(
			Dim cellDimensions)
			: m_cellDimensions(cellDimensions)
		{}

		void Insert(
			const T& item,
			const Dim& position,
			const AABB& bounds)
		{
			Key key = GetKey<Dim>(position, bounds);
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
		Key GetKey(
			const Dim& position,
			const AABB& bounds);

		template<>
		Key GetKey<iwm::vector2>(
			const Dim& position,
			const AABB& bounds)
		{
			iwm::vector3 key = (bounds.Center() + position) / m_cellDimensions;
			Key x = (Key)key.x;
			Key y = (Key)key.y;

			constexpr static unsigned char KEYPART = sizeof(Key) * 4 / 2;

			return x << (KEYPART * 0)
				+ y << (KEYPART * 1);
		}

		template<>
		Key GetKey<iwm::vector3>(
			const Dim& position,
			const AABB& bounds)
		{
			iwm::vector3 key = (bounds.Center() + position) / m_cellDimensions;
			Key x = (Key)key.x;
			Key y = (Key)key.y;
			Key z = (Key)key.z;

			constexpr static unsigned char KEYPART = sizeof(Key) * 4 / 3;

			return x << (KEYPART * 0)
				+ y << (KEYPART * 1)
				+ z << (KEYPART * 2);
		}

		template<>
		Key GetKey<iwm::vector4>(
			const Dim& position,
			const AABB& bounds)
		{
			iwm::vector3 key = (bounds.Center() + position) / m_cellDimensions;
			Key x = (Key)key.x;
			Key y = (Key)key.y;
			Key z = (Key)key.z;
			Key w = (Key)key.w;

			constexpr static unsigned char KEYPART = sizeof(Key) * 4 / 4;

			return x << (KEYPART * 0)
				+ y << (KEYPART * 1)
				+ z << (KEYPART * 2)
				+ z << (KEYPART * 3);
		}
	};
}
