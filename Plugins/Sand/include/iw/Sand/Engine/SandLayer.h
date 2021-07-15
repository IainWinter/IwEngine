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

	//IW_PLUGIN_SAND_API
	//inline void FillPolygon(
	//	const std::vector<glm::vec2>& polygon,
	//	const std::vector<unsigned>& index,
	//	const iw::Cell& cell)
	//{
	//	ForEachInPolygon(polygon, index, [&](int x, int y, int u, int v) {
	//		m_world->SetCell(x, y, cell);
	//	});
	//}

	//IW_PLUGIN_SAND_API
	//void ForEachInPolygon(
	//	const std::vector<glm::vec2>& polygon,
	//	const std::vector<unsigned>& index,
	//	std::function<void(int, int, int, int)> func);

	IW_PLUGIN_SAND_API
	void ForEachInPolygon(
		const std::vector<glm::vec2>& polygon,
		const std::vector<glm::vec2>& uv,
		const std::vector<unsigned>& index,
		std::function<void(int, int, float, float)> func);

// small software renderer, could take out and make its own class

private:
	struct Vertex {
		glm::vec2 pos;
		glm::vec2 uv;
	};

	struct InterpolationUV { // could template
		float m_stepUX;
		float m_stepUY;
		float m_stepVX;
		float m_stepVY;

		InterpolationUV(
			const Vertex& v1,
			const Vertex& v2,
			const Vertex& v3)
		{
			const auto& [p1, u1] = v1; // u is getting interpolated
			const auto& [p2, u2] = v2;
			const auto& [p3, u3] = v3;

			const float& x1 = p1.x, y1 = p1.y; // for interpolation amount
			const float& x2 = p2.x, y2 = p2.y;
			const float& x3 = p3.x, y3 = p3.y;

			float dx = (x2-x3)*(y1-y3) - (x1-x3)*(y2-y3);
			float dy = -dx;

			glm::vec2 dCdx = ( (u2-u3)*(y1-y3) - (u1-u3)*(y2-y3) ) / dx;
			glm::vec2 dCdy = ( (u2-u3)*(x1-x3) - (u1-u3)*(x2-x3) ) / dy;

			m_stepUX = dCdx.x; m_stepUY = dCdx.y;
			m_stepVX = dCdy.x; m_stepVY = dCdy.y;
		}
	};

	struct Edge {
		float m_x, m_u, m_v;
		float m_stepX, m_stepU, m_stepV;
		int m_minY, m_maxY;

		Edge(
			const Vertex& v1,
			const Vertex& v2,
			const InterpolationUV& uvLerp)
		{
			const float& minX = v1.pos.x, minY = v1.pos.y;
			const float& maxX = v2.pos.x, maxY = v2.pos.y;

			m_minY = ceil(minY);
			m_maxY = ceil(maxY);

			float preStepY = m_minY - minX;

			float distX = maxX - minX;
			float distY = maxY - minY;

			m_stepX = distX / distY; // divs by 0 but doesnt matter
			m_x = minX + preStepY * m_stepX;

			float preStepX = m_x - minY;

			m_u = v1.uv.x + uvLerp.m_stepUX * preStepX + uvLerp.m_stepUY * preStepY;
			m_v = v1.uv.y + uvLerp.m_stepVX * preStepX + uvLerp.m_stepVY * preStepY;

			m_stepU = uvLerp.m_stepUY + uvLerp.m_stepUX * m_stepX;
			m_stepV = uvLerp.m_stepVY + uvLerp.m_stepVX * m_stepX;
		}

		void Step() {
			m_x += m_stepX;
			m_u += m_stepU;
			m_v += m_stepV;
		}
	};

	IW_PLUGIN_SAND_API
	void ScanTriangle(
		Vertex v1,
		Vertex v2,
		Vertex v3,
		std::function<void(int, int, float, float)>& func);
};

IW_PLUGIN_SAND_END
