#pragma once

#include "iw/graphics/Color.h"
#include "iw/util/enum/val.h"

enum class CellProperties {
	NONE                = 0b00000000,
	MOVE_DOWN           = 0b00000001,
	MOVE_DOWN_SIDE      = 0b00000010,
	MOVE_SIDE           = 0b00000100
};

enum class CellType {
	EMPTY,
	SAND,
	WATER,
	ROCK
};

struct Cell {
	CellType       Type  = CellType::EMPTY;
	CellProperties Props = CellProperties::NONE;

	iw::Color Color; // rgba
};

inline CellProperties operator|(
	CellProperties a, CellProperties b)
{
	return CellProperties(iw::val(a) | iw::val(b));
}

inline auto operator&(
	CellProperties a, CellProperties b)
{
	return iw::val(a) & iw::val(b);
}

//#include <unordered_map>

//	int LastUpdateTick = 0;
//
//	// Static defaults
//
//	static void SetDefault(CellType type, const Cell& cell) {
//		m_defaults.emplace(type, cell);
//	}
//
//	static const Cell& GetDefault(CellType type) {
//		return m_defaults.at(type);
//	}
//private:
//	static std::unordered_map<CellType, Cell> m_defaults;
