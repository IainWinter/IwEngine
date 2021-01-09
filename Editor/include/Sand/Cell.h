#pragma once

#include "iw/graphics/Color.h"
#include "iw/util/enum/val.h"
#include <unordered_map>
#include <mutex>

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
	SMOKE,
	EXPLOSION,

	LASER,
	eLASER,
	BULLET,
	MISSILE
};

enum class CellProperties {
	NONE                = 0b0000000000000000,
	MOVE_DOWN           = 0b0000000000000001,
	MOVE_DOWN_SIDE      = 0b0000000000000010,
	MOVE_SIDE           = 0b0000000000000100,
	MOVE_RANDOM         = 0b0000000000001000,
	MOVE_FORWARD        = 0b0000000000010000,
	MOVE_SHARED_USER    = 0b0000000000100000,
	HIT_LIKE_PROJECTILE = 0b0000000001000000,
	HIT_LIKE_BEAM       = 0b0000000010000000,
	HIT_LIKE_MISSILE    = 0b0000000100000000,
	DELETE_TIME         = 0b0000001000000000,
};
inline CellProperties operator|(CellProperties a,CellProperties b){return CellProperties(iw::val(a)|iw::val(b));}
inline auto           operator&(CellProperties a,CellProperties b){return iw::val(a)&iw::val(b);}

enum class SharedCellType {
	NONE,
	ASTEROID
};

struct SharedCellData {
	float pX = 0; // Position
	float pY = 0;
	float angle = 0;

	float vX = 0; // Velocity
	float vY = 0;

	float Speed = 0;
	float Timer = 0;

	float cX = 0; // Center
	float cY = 0;

	SharedCellType Type = SharedCellType::NONE;

	void* Special = nullptr;

	bool Hit = false;
	int hX = 0;
	int hY = 0;

	std::mutex m_userCountMutex;
	int UserCount = 0;

	// uhg

	SharedCellData():m_userCountMutex(){}
	SharedCellData(const SharedCellData& copy)
		: pX(copy.pX)
		, pY(copy.pY)
		, angle(copy.angle)
		, vX(copy.vX)
		, vY(copy.vY)
		, cX(copy.cX)
		, cY(copy.cY)
		, Type(copy.Type)
		, Special(copy.Special)
		, m_userCountMutex()
		, UserCount(copy.UserCount)
	{}
	SharedCellData& operator=(const SharedCellData& copy){
		pX        = copy.pX;
		pY        = copy.pY;
		angle     = copy.angle;
		vX        = copy.vX;
		vY        = copy.vY;
		cX        = copy.cX;
		cY        = copy.cY;
		Type      = copy.Type;
		Special = copy.Special;
		UserCount = copy.UserCount;
		return *this;
	}
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
	SharedCellData* User = nullptr; // Shared data

	int Precedence = 10;

	float Speed() const {  // Manhattan distance of velocity
		return sqrt(dX*dX + dY*dY);
	}

	static inline void        SetDefault(CellType type,const Cell& cell){m_defaults.emplace(type, cell);}
	static inline const Cell& GetDefault(CellType type)                 {return m_defaults.at(type);}
private:
	static inline std::unordered_map<CellType, Cell> m_defaults;
};
