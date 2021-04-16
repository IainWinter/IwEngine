#pragma once

#include "iw/graphics/Color.h"
#include <unordered_map>

#define IW_PLUGIN_SAND_BEGIN namespace iw {namespace plugins {namespace Sand {
#define IW_PLUGIN_SAND_END }using namespace Sand; }using namespace plugins; }

IW_PLUGIN_SAND_BEGIN

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_PLUGIN_SAND_DLL
#		define IW_PLUGIN_SAND_API __declspec(dllexport)
#	else
#		define IW_PLUGIN_SAND_API __declspec(dllimport)
#	endif
#else
#	define IW_PLUGIN_SAND_API
#endif

enum class CellType {
	EMPTY,

	SAND,
	WATER,
	ROCK,

	WOOD, FIRE, SMOKE
};

enum class CellProperties {
	NONE           = 0b0000000000000000,
	MOVE_DOWN      = 0b0000000000000001,
	MOVE_DOWN_SIDE = 0b0000000000000010,
	MOVE_SIDE      = 0b0000000000000100,
	MOVE_RANDOM    = 0b0000000000001000,
	MOVE_FORCE     = 0b0000000000010000,
	BURN           = 0b0000000000100000
};
inline CellProperties operator|(CellProperties a,CellProperties b){return CellProperties(int(a)|int(b));}
inline auto           operator&(CellProperties a,CellProperties b){return int(a)&int(b);}

struct SharedCellData;

struct Cell {
	CellType       Type  = CellType::EMPTY;
	CellProperties Props = CellProperties::NONE;
	
	iw::Color Color;

	float x, y, dx, dy, time;

	static inline void        SetDefault(CellType type,const Cell& cell){m_defaults.emplace(type, cell);}
	static inline const Cell& GetDefault(CellType type)                 {return m_defaults.at(type);}
private:
	static inline std::unordered_map<CellType, Cell> m_defaults;
};

IW_PLUGIN_SAND_END