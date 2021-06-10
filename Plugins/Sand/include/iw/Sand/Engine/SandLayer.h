#pragma once

#include "iw/engine/Layer.h"

#include "../Workers/SimpleSandWorker.h"
#include "SandUpdateSystem.h"
#include "SandRenderSystem.h"

IW_PLUGIN_SAND_BEGIN

class SandLayer
	: public Layer
{
public:
	SandWorld* m_world;
	const int m_cellSize;
	const int m_cellsPerMeter;
private:
	SandWorldRenderSystem* m_render;
	SandWorldUpdateSystem* m_update;

	bool m_drawMouseGrid;

	int gridSize = 16;
	vec2 sP, gP; // sand pos, grid pos

public:
	SandLayer(
		int cellSize,
		int cellsPerMeter,
		bool drawMouseGrid = false
	)
		: Layer("Sand")
		, m_cellSize(cellSize)
		, m_cellsPerMeter(cellsPerMeter)
		, m_drawMouseGrid(drawMouseGrid)
		, m_world (nullptr)
		, m_render(nullptr)
		, m_update(nullptr)
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

	void SetCamera(
		float x,  float y,
		float xs, float ys)
	{
		int width  = m_render->GetSandTexture()->Width()  / 2;
		int height = m_render->GetSandTexture()->Height() / 2;

		x = ceil(x * xs);
		y = ceil(y * ys);

		m_render->SetCamera(x + -width, y + -height, x + width, y + height);
		m_update->SetCameraScale(xs, ys);
	}

	IW_PLUGIN_SAND_API
	Entity MakeTile(
		const std::string& sprite,
		bool isStatic = false,
		bool isSimulated = false);

	IW_PLUGIN_SAND_API
	void FillPolygon(
		const std::vector<glm::vec2>& polygon,
		const std::vector<unsigned>&  index);
};

IW_PLUGIN_SAND_END
