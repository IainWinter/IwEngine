#pragma once

#include "SandWorld.h"

IW_PLUGIN_SAND_BEGIN

class SandWorker {
protected:
	SandWorld& m_world;
	SandChunk* m_chunk;

public:
	IW_PLUGIN_SAND_API SandWorker(SandWorld& world, SandChunk* chunk);

	IW_PLUGIN_SAND_API void UpdateChunk();

	virtual void UpdateCell(int x, int y, Cell& cell) = 0;

	IW_PLUGIN_SAND_API Cell& GetCell(int x, int y);

	IW_PLUGIN_SAND_API void SetCell (int x, int y, const Cell& cell);
	IW_PLUGIN_SAND_API void MoveCell(int x, int y, int xto, int yto);

	IW_PLUGIN_SAND_API void KeepAlive(int x, int y);

	IW_PLUGIN_SAND_API bool InBounds(int x, int y);
	IW_PLUGIN_SAND_API bool IsEmpty (int x, int y);

	inline void ShuffleIfTrue(bool& a, bool& b) {
		if (a && b) {
			a = glm::linearRand(0.f, 1.f) > 0;
			b = !a;
		}
	}
};

IW_PLUGIN_SAND_END
