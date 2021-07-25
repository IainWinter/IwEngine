#pragma once

#include "../SandWorker.h"
#include "iw/engine/Time.h"

IW_PLUGIN_SAND_BEGIN

class SimpleSandWorker : public SandWorker {
public:
	IW_PLUGIN_SAND_API SimpleSandWorker(SandWorld& world, SandChunk* chunk);

	IW_PLUGIN_SAND_API void UpdateCell(int x, int y, Cell& cell) override;
private:
	IW_PLUGIN_SAND_API bool MoveForce   (int x, int y,       Cell& cell);
	IW_PLUGIN_SAND_API bool MoveDown    (int x, int y, const Cell& cell);
	IW_PLUGIN_SAND_API bool MoveDownSide(int x, int y, const Cell& cell);
	IW_PLUGIN_SAND_API bool MoveSide    (int x, int y, const Cell& cell);
	IW_PLUGIN_SAND_API bool MoveRandom  (int x, int y, const Cell& cell);
};

template<>
struct SandWorkerBuilder<SimpleSandWorker>
	: SandWorkerBuilderBase
{
	ref<SandWorker> MakeWorker(SandWorld& world, SandChunk* chunk) {
		return REF<SimpleSandWorker>(world, chunk);
	}
};

IW_PLUGIN_SAND_END
