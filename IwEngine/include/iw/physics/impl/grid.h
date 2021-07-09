#pragma once

#include "iw/physics/impl/AABB.h"
#include "iw/util/algorithm/pair_hash.h"
#include "glm/gtx/hash.hpp"
#include <unordered_map>

namespace iw {
namespace Physics {
namespace impl {

	// stores a copy of each item in each grid cell it overlaps
	template<
		typename _t,
		Dimension _d = Dimension::d2>
	class grid
	{
	public:
		std::unordered_map<std::pair<int, int>, std::vector<_t>, iw::pair_hash> m_grid;
	private:
		_vec<_d> m_cellSize;

	public:
		grid(const AABB<_d>& cellBounds)
			: m_cellSize(glm::compAdd(cellBounds.Max, -cellBounds.Min))
		{}

		grid(const _vec<_d>& cellSize)
			: m_cellSize(cellSize)
		{
			assert(cellSize.x > 0 || cellSize.y > 0);
		}

		grid(int cellSize)
			: m_cellSize(cellSize)
		{
			assert(cellSize > 0);
		}

		//grid() = default;
		//grid& operator=(const grid& grid) = default; // because of const?

		GEN_default6(, grid) // cellSize = (-INF, INF) -> there is only 1 cell

		void foreach(
			const AABB<_d>& bounds,
			std::function<void(std::pair<int, int>, std::vector<_t>&)> func)
		{
			assert( bounds.Min != AABB<_d>().Min
				&& bounds.Max != AABB<_d>().Max);

			_ivec<_d> minPos = glm::floor(bounds.Min / m_cellSize);
			_ivec<_d> maxPos = glm::floor(bounds.Max / m_cellSize);

			for (int x = minPos.x; x <= maxPos.x; x++)
			for (int y = minPos.y; y <= maxPos.y; y++)
			{
				auto p = std::make_pair(x, y);
				func(p, m_grid[p]);
			}
		}

		void emplace(
			const _t& item,
			const AABB<_d>& bounds)
		{
			foreach(bounds, [&](auto p, auto& items) {
				items.push_back(item);
			});
		}

		void erase(
			const _t& item,
			const AABB<_d>& hint = AABB<_d>())
		{
			foreach(hint, [&](auto p, auto& items) {
				items.erase(std::find(items.begin(), items.end(), item));
			});
		}

		void clear() {
			m_grid.clear();
		}
	};
}
}

	using namespace Physics;
}
