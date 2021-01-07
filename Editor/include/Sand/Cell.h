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
	NONE                = 0b0000000000000000,
	MOVE_DOWN           = 0b0000000000000001,
	MOVE_DOWN_SIDE      = 0b0000000000000010,
	MOVE_SIDE           = 0b0000000000000100,
	MOVE_FORWARD        = 0b0000000000001000,
	MOVE_SHARED_USER    = 0b0000000000010000,
	HIT_LIKE_PROJECTILE = 0b0000000000100000,
	HIT_LIKE_BEAM       = 0b0000000001000000,
	HIT_LIKE_MISSILE    = 0b0000000010000000,
};
inline CellProperties operator|(CellProperties a,CellProperties b){return CellProperties(iw::val(a)|iw::val(b));}
inline auto           operator&(CellProperties a,CellProperties b){return iw::val(a)&iw::val(b);}

enum class SharedCellType {
	NONE,
	ASTEROID,
	MISSILE
};

struct SharedCellData {
	float pX = 0; // Position
	float pY = 0;
	float angle = 0;

	float vX = 0; // Velocity
	float vY = 0;

	float cX = 0; // Center
	float cY = 0;

	SharedCellType Type = SharedCellType::NONE;
};

struct Cell {
	CellType       Type  = CellType::EMPTY;
	CellProperties Props = CellProperties::NONE;

	iw::Color Color;

	float Life = 0; // Life until the cell will die, only some cells use this
	float pX = 0;   // 'Position'
	float pY = 0;
	float dX = 0;   // Velocity
	float dY = 0;

	int SplitCount = 0; // to stop lazers and bullets from splitting to much

	int TileId = 0;          // Tile id, 0 means that it belongs to noone
	int LastUpdateTick = 0;  // Used to check if the cell has been updated in the current tick
	bool Gravitised = false; // If this cell should react to gravity
	SharedCellData* User = nullptr;    // Any data from user

	float Speed() const {  // Manhattan distance of velocity
		return sqrt(dX*dX + dY*dY);
	}

	static inline void        SetDefault(CellType type,const Cell& cell){m_defaults.emplace(type, cell);}
	static inline const Cell& GetDefault(CellType type)                 {return m_defaults.at(type);}
private:
	static inline std::unordered_map<CellType, Cell> m_defaults;
};
