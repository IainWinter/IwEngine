#pragma once

#include "iw/graphics/Color.h"
#include "iw/util/enum/val.h"
#include <unordered_map>

using Tick = size_t;

using Index = unsigned int;
using IndexPair = std::pair<Index, Index>;

using ChunkCoord  = /*unsigned*/ int;
using WorldCoord  = int;
using ChunkCoords = std::pair<ChunkCoord, ChunkCoord>;
using WorldCoords = std::pair<WorldCoord, WorldCoord>;

enum class CellType {
	EMPTY,

	SAND,
	WATER,
	ROCK,
	METAL,
	DEBRIS,

	LASER,
	eLASER,
	BULLET
};

enum class CellProperties {
	NONE                = 0b00000000,
	MOVE_DOWN           = 0b00000001,
	MOVE_DOWN_SIDE      = 0b00000010,
	MOVE_SIDE           = 0b00000100,
	MOVE_FORWARD        = 0b00001000,
	HIT_LIKE_PROJECTILE = 0b00010000,
	HIT_LIKE_BEAM       = 0b00100000
};
inline CellProperties operator|(CellProperties a,CellProperties b){return CellProperties(iw::val(a)|iw::val(b));}
inline auto           operator&(CellProperties a,CellProperties b){return iw::val(a)&iw::val(b);}

struct Cell {
	CellType       Type  = CellType::EMPTY;
	CellProperties Props = CellProperties::NONE;

	iw::Color Color;

	float Life = 0; // Life until the cell will die, only some cells use this
	float pX =  0;  // 'Position'
	float pY =  0;
	float dX =  0;  // Velocity
	float dY = -1;

	int TileId = 0;          // Tile id, 0 means that it belongs to noone
	int LastUpdateTick = 0;  // Used to check if the cell has been updated in the current tick
	bool Gravitised = false; // If this cell should react to gravity

	float Speed() const {  // Manhattan distance of velocity
		return (dX > 0 ? dX : -dX)
			+ (dY > 0 ? dY : -dY);
	}

	static inline void        SetDefault(CellType type,const Cell& cell){m_defaults.emplace(type, cell);}
	static inline const Cell& GetDefault(CellType type)                 {return m_defaults.at(type);}
private:
	static inline std::unordered_map<CellType, Cell> m_defaults;
};
