#pragma once

#include "iw/graphics/Color.h"
#include "iw/util/enum/val.h"
#include "iw/math/vector2.h"
#include <unordered_map>
#include <functional>
#include <mutex>
#include <set>

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

	REZ,

	LASER,
	eLASER,
	mLASER,
	BULLET,
	MISSILE,


	DEBUG
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

struct SharedCellData;

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
	int MaxSplitCount = 0;

	int TileId = 0;          // Tile id, 0 means that it belongs to noone
	int LastUpdateTick = 0;  // Used to check if the cell has been updated in the current tick
	bool Gravitised = false; // If this cell should react to gravity
	SharedCellData* Share = nullptr; // Shared data

	int Precedence = 20;

	bool UseFloatingPosition = false;

	float Speed() const {  // Manhattan distance of velocity
		return sqrt(dX*dX + dY*dY);
	}

	static inline void        SetDefault(CellType type,const Cell& cell){m_defaults.emplace(type, cell);}
	static inline const Cell& GetDefault(CellType type)                 {return m_defaults.at(type);}
private:
	static inline std::unordered_map<CellType, Cell> m_defaults;
};

enum class SharedCellType {
	NONE,
	ASTEROID
};

struct SharedCellData {
	bool UsedForMotion = false;

	bool RecordHitCells = false;
	std::vector<std::tuple<WorldCoords, Cell>> HitCells;
	std::mutex m_hitCellsMutex;

	void RecordHit(WorldCoord x, WorldCoord y, const Cell& cell) {
		std::unique_lock lock(m_hitCellsMutex);
		HitCells.emplace_back(std::make_pair(x, y), cell);
	}

	float Life = 0;

	float pX = 0; // Position
	float pY = 0;
	float angle = 0;

	float vX = 0; // Velocity
	float vY = 0;

	//float Speed = 0;
	//float Timer = 0;

	float cX = 0; // Center
	float cY = 0;

	SharedCellType Type = SharedCellType::NONE;

	void* Special = nullptr;

	bool Hit = false;
	int hX = 0;
	int hY = 0;

	int UserCount = 0; // Total count
	std::unordered_map<CellType, std::pair<int, iw::vector2>> UserTypeCounts; // Count of each type, avg location
	std::set<Cell*> m_users; // for cleanup
	std::mutex m_userMutex;

	float Radius() {
		iw::vector2 furthest;

		for (Cell* cell : m_users) {
			iw::vector2 v(cell->pX-pX, cell->pY-pY);
			if (v.length_squared() > furthest.length_squared()) {
				furthest = v;
			}
		}

		return furthest.length();
	}

	// uhg

	SharedCellData() {}
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
		, UserCount(copy.UserCount)
		, UserTypeCounts(copy.UserTypeCounts)
		, UsedForMotion(copy.UsedForMotion)
		, RecordHitCells(copy.RecordHitCells)
		, HitCells(copy.HitCells)
		, m_users(copy.m_users)
	{}
	SharedCellData& operator=(const SharedCellData& copy){
		pX = copy.pX;
		pY = copy.pY;
		angle = copy.angle;
		vX = copy.vX;
		vY = copy.vY;
		cX = copy.cX;
		cY = copy.cY;
		Type  = copy.Type;
		Special = copy.Special;
		UserCount = copy.UserCount;
		UserTypeCounts = copy.UserTypeCounts;
		UsedForMotion = copy.UsedForMotion;
		RecordHitCells = copy.RecordHitCells;
		HitCells = copy.HitCells;
		m_users = copy.m_users;
		return *this;
	}
};
