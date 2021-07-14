#pragma once

#include "iw/engine/Layer.h"

#include "../Workers/SimpleSandWorker.h"
#include "SandUpdateSystem.h"
#include "SandRenderSystem.h"
#include "iw/physics/spacial/grid.h"

IW_PLUGIN_SAND_BEGIN

class SandLayer
	: public Layer
{
public:
	SandWorld* m_world;
	const int m_cellSize;
	const int m_cellsPerMeter;

	//std::unordered_map<std::pair<int, int>, ref<RenderTarget>, iw::pair_hash> m_chunkTextures; // for rendering tiles into world
	//grid2<Tile*> m_tiles;

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
		, m_world(nullptr)
		, m_render(nullptr)
		, m_update(nullptr)
	{}

	IW_PLUGIN_SAND_API int  Initialize();
	IW_PLUGIN_SAND_API void PreUpdate();
	IW_PLUGIN_SAND_API void PostUpdate();

	IW_PLUGIN_SAND_API void DrawMouseGrid();
	IW_PLUGIN_SAND_API void DrawWithMouse(int fx, int fy, int width, int height);

	IW_PLUGIN_SAND_API bool On(MouseWheelEvent& e);

	IW_PLUGIN_SAND_API void PasteTiles();
	IW_PLUGIN_SAND_API void RemoveTiles();

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
	inline Entity MakeTile(
		const std::string& sprite,
		bool isSimulated = false)
	{
		return MakeTile(REF<Texture>(16, 64), isSimulated);
	}

	IW_PLUGIN_SAND_API
	Entity MakeTile(
		ref<Texture>& sprite,
		bool isSimulated = false);

	IW_PLUGIN_SAND_API
	inline void FillPolygon(
		const std::vector<glm::vec2>& polygon,
		const std::vector<unsigned>& index,
		const iw::Cell& cell)
	{
		ForEachInPolygon(polygon, index, [&](int x, int y, int u, int v) {
			m_world->SetCell(x, y, cell);
		});
	}

	IW_PLUGIN_SAND_API
	void ForEachInPolygon(
		const std::vector<glm::vec2>& polygon,
		const std::vector<unsigned>& index,
		std::function<void(float, float, int, int)> func)
	{
		ForEachInPolygon(polygon, {}, index, func);
	}

	IW_PLUGIN_SAND_API
	void ForEachInPolygon(
		const std::vector<glm::vec2>& polygon,
		const std::vector<glm::vec2>& uv,
		const std::vector<unsigned>& index,
		std::function<void(int, int, int, int)> func);

// small software renderer, could take out and make its own class

private:
	struct Edge {
		float m_x;
		float m_stepX;
		int m_minY;
		int m_maxY;

		Edge(glm::vec2 minY, glm::vec2 maxY)
		{
			glm::vec2 dist = maxY - minY;

			m_minY = ceil(minY.y);
			m_maxY = ceil(maxY.y);
			m_stepX = dist.x / dist.y;
			m_x = minY.x + (m_minY - minY.y) * m_stepX;
		}

		void Step() {
			m_x += m_stepX;
		}
	};

	IW_PLUGIN_SAND_API
	void ScanTriangle(
		glm::vec2 v1,
		glm::vec2 v2,
		glm::vec2 v3,
		std::function<void(int, int, int, int)>& func);
};

IW_PLUGIN_SAND_END
