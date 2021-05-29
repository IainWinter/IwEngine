#pragma once

#include "iw/engine/Layer.h"

#include "../Workers/SimpleSandWorker.h"
#include "SandUpdateSystem.h"
#include "SandRenderSystem.h"

IW_PLUGIN_SAND_BEGIN

class SandLayer
	: public Layer
{
private:
	SandWorld* m_world;
	SandWorldRenderSystem* m_render;

	bool m_drawMouseGrid;

	int gridSize = 16;
	vec2 sP, gP; // sand pos, grid pos

public:
	SandLayer(
		bool drawMouseGrid = false
	)
		: Layer("Sand")
		, m_world (nullptr)
		, m_render(nullptr)
		, m_drawMouseGrid(drawMouseGrid)
	{}

	IW_PLUGIN_SAND_API int  Initialize();
	IW_PLUGIN_SAND_API void PreUpdate();
	IW_PLUGIN_SAND_API void PostUpdate();

	IW_PLUGIN_SAND_API void DrawMouseGrid();
	IW_PLUGIN_SAND_API void DrawWithMouse(int fx, int fy, int width, int height);

	IW_PLUGIN_SAND_API bool On(MouseWheelEvent& e);

	Mesh& GetSandMesh() {
		return m_render->GetSandMesh();
	}

	IW_PLUGIN_SAND_API Entity MakeTile(const std::string& sprite, bool isStatic = false, bool isSimulated = false);
};

IW_PLUGIN_SAND_END
