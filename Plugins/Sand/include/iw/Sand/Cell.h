#pragma once

#include "iw/graphics/Color.h"
#include "iw/math/iwmath.h"
#include <unordered_map>
#include <functional>

#include "Defs.h"

IW_PLUGIN_SAND_BEGIN

struct Cell {
	CellType  Type  = CellType::EMPTY;
	CellProp  Props = CellProp::NONE;
	CellStyle Style = CellStyle::NONE;
	
	iw::Color Color;
	iw::Color StyleColor;

	float StyleOffset = 0;

	float x = 0, y = 0, dx = 0, dy = 0;
	float life = 0;

	std::function<void(iw::Cell&)> OnUpdate;

	static inline void SetDefault(
		CellType type,
		const Cell& cell)
	{
		m_defaults.emplace(type, cell);
	}

	static inline const Cell& GetDefault(
		CellType type)
	{
		Cell& cell = m_defaults.at(type);
		cell.StyleOffset = iw::randfs();
		return cell;
	}

	bool operator==(const Cell& other) const {
		return Type == other.Type;
	}

	bool operator!=(const Cell& other) const {
		return !operator==(other);
	}
private:
	IW_PLUGIN_SAND_API
	static inline std::unordered_map<CellType, Cell> m_defaults;
};

IW_PLUGIN_SAND_END
