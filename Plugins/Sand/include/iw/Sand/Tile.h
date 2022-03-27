#pragma once

#include "Defs.h"
#include "iw/graphics/Texture.h"
#include "iw/common/Components/Transform.h"
#include "iw/common/algos/polygon2.h"
#include "iw/physics/AABB.h"
#include <functional>
#include <utility>

IW_PLUGIN_SAND_BEGIN

// Scaling transforms is not supported...

struct Tile {

	// Texture that gets drawn,
	// if a pixel has an alpha of 0 it's not draw
	// if a pixel has an alpha of 0.5 it's disabled / ejected

	Texture m_sprite;
	iw::Color m_tint = iw::Color(1.f);
	//bool m_drawAsBox = false;
	bool m_dontRemoveCells = false;

	enum PixelState : unsigned char {
		EMPTY   = 0x00,
		REMOVED = 0x80,
		FILLED  = 0xFF
	};

	// put these funcs in cpp

	inline static PixelState cState(unsigned color)
	{
		return (PixelState)(color >> 24);
	}

	PixelState State(unsigned index)
	{
		return cState(m_sprite.Colors32()[index]);
	}

	void SetState(unsigned index, PixelState state)
	{
		char& current = *(char*)(&m_sprite.Colors()[index * 4] + 3);
		NeedsScan = current != (char)state;
		current = (char)state;
	}

	bool RemovePixel(unsigned index)
	{
		if (m_dontRemoveCells) return false;

		bool remove = State(index) == FILLED;

		if (remove)
		{
			m_currentCellCount--;
			m_removedCells.push_back(index);
			m_justRemovedCells.push_back(index);
			m_currentCells.erase(std::find(m_currentCells.begin(), m_currentCells.end(), index));
			SetState(index, REMOVED);
		}

		return remove;
	}

	bool ReinstatePixel(unsigned index)
	{
		bool reinstate = State(index) == REMOVED;

		if (reinstate)
		{
			m_currentCellCount++;
			m_removedCells.erase(std::find(m_removedCells.begin(), m_removedCells.end(), index));
			m_currentCells.push_back(index);
			SetState(index, FILLED);

			// remove from just removed if needed, this gets cleared in RemoveTiles btw
			auto itr = std::find(m_justRemovedCells.begin(), m_justRemovedCells.end(), index);
			if (itr != m_justRemovedCells.end())
			{
				m_justRemovedCells.erase(itr);
			}
		}

		return reinstate;
	}

	bool ReinstateRandomPixel()
	{
		size_t size = m_removedCells.size();
		if (size == 0) return false;

		return ReinstatePixel(m_removedCells.at(iw::randi(size - 1u)));
	}

	// It seems like thm_removedCells.size() etter if the rendererd geometry is just a square
	// The physical colliders should still use scanned one obviously, but many more triangles can be removed

	std::vector<glm::vec2> m_polygon; // for raster
	std::vector<glm::vec2> m_uv;
	std::vector<unsigned>  m_index;

	std::vector<glm::vec2> m_collider; // for physics
	std::vector<unsigned>  m_colliderIndex;

	AABB2 m_bounds;

	Transform LastTransform;
	bool IsStatic = false;
	bool NeedsDraw = true;
	bool NeedsScan = true;

	int m_initalCellCount  = -1;
	int m_currentCellCount = -1;

	std::vector<int> m_currentCells;
	std::vector<int> m_removedCells;
	std::vector<int> m_justRemovedCells; // removed this frame to get rid of tile info, clearend in RemoveTiles

	int m_sandLayerIndex = 0;
	int m_zIndex = 0;

	IW_PLUGIN_SAND_API
	Tile() = default;

	IW_PLUGIN_SAND_API
	Tile(const Texture& texture);

	IW_PLUGIN_SAND_API
	void UpdateColliderPolygon();
};

IW_PLUGIN_SAND_END
