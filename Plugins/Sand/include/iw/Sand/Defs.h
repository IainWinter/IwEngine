#pragma once

#define IW_PLUGIN_SAND_BEGIN namespace iw {namespace plugins {namespace Sand {
#define IW_PLUGIN_SAND_END }using namespace Sand; }using namespace plugins; }

#ifdef IW_PLATFORM_WINDOWS
#	ifdef IW_PLUGIN_SAND_DLL
#		define IW_PLUGIN_SAND_API __declspec(dllexport)
#	else
#		define IW_PLUGIN_SAND_API __declspec(dllimport)
#	endif
#else
#	define IW_PLUGIN_SAND_API
#endif

IW_PLUGIN_SAND_BEGIN

struct Tile;

struct TileInfo {
	Tile* tile;
	unsigned index;
	bool operator==(const TileInfo& other) const { return tile == other.tile && index == other.index; }
	bool operator!=(const TileInfo& other) const { return !operator==(other); }
};

enum class CellType {
	EMPTY,

	SAND,
	WATER,
	ROCK,
	STONE,

	WOOD, FIRE, SMOKE, BELT,

	#include "config/CellType.def"
};

enum class CellStyle {
	NONE,
	RANDOM_STATIC,
	SHIMMER,

	#include "config/CellStyle.def"
};

enum class CellProp {
	NONE           = 0b0000000000000000,
	MOVE_DOWN      = 0b0000000000000001,
	MOVE_DOWN_SIDE = 0b0000000000000010,
	MOVE_SIDE      = 0b0000000000000100,
	MOVE_RANDOM    = 0b0000000000001000,
	MOVE_FORCE     = 0b0000000000010000,

	BURN     = 0b0000000000100000,
	CONVEYOR = 0b0000000001000000,

	NONE_TILE = 0b1000000000000000,

	#include "config/CellProp.def"
};

inline CellProp operator|(CellProp a,CellProp b){return CellProp(int(a)|int(b));}
inline auto     operator&(CellProp a,CellProp b){return int(a)&int(b);}

enum class SandField : unsigned { // Should allow user to add to this, see enum { #include "fields.h" }; idea from valve
	CELL,
	SOLID, // For tile textures making it so other cells know about tiles
	COLOR, // For tile textures
	TILE_INFO, // For Tile* and sprite pixel index

	//FIELD_COUNT
};

// for platformer other game
//CELL,
//SOLID,
//COLLISION,

IW_PLUGIN_SAND_END
