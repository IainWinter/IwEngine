#pragma once

#include "../SandWorker.h"

IW_PLUGIN_SAND_BEGIN

class SimpleSandWorker : public SandWorker {
public:
	IW_PLUGIN_SAND_API SimpleSandWorker(SandWorld& world, SandChunk* chunk);

	IW_PLUGIN_SAND_API void UpdateCell(int x, int y, Cell& cell) override;
private:
	IW_PLUGIN_SAND_API bool MoveDown    (size_t x, size_t y, const Cell& cell);
	IW_PLUGIN_SAND_API bool MoveDownSide(size_t x, size_t y, const Cell& cell);
	IW_PLUGIN_SAND_API bool MoveSide    (size_t x, size_t y, const Cell& cell);
};

IW_PLUGIN_SAND_END
