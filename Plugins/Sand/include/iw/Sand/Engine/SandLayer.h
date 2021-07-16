#pragma once

#include "iw/engine/Layer.h"

#include "../Workers/SimpleSandWorker.h"
#include "SandUpdateSystem.h"
#include "SandRenderSystem.h"
#include <array>

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

	std::vector<std::pair<Transform*, Tile*>> m_tilesThisFrame;

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

	IW_PLUGIN_SAND_API void PasteTiles (const std::vector<std::pair<Transform*, Tile*>>& tiles);
	IW_PLUGIN_SAND_API void RemoveTiles(const std::vector<std::pair<Transform*, Tile*>>& tiles);

	IW_PLUGIN_SAND_API
	void ForEachTile(
		const std::vector<std::pair<Transform*, Tile*>>& tiles,
		std::function<void(Transform*, Tile*)> func);

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
	void SandLayer::ForEachInPolygon(
		const std::vector<glm::vec2>& polygon,
		const std::vector<glm::vec2>& uv,
		const std::vector<unsigned>&  index,
		std::function<void(int, int, float, float)> func)
	{
		for (size_t i = 0; i < index.size(); i += 3) {
			const glm::vec2& p1 = polygon[index[i]];
			const glm::vec2& p2 = polygon[index[i + 1]];
			const glm::vec2& p3 = polygon[index[i + 2]];
		
			const glm::vec2& u1 = uv[index[i]];
			const glm::vec2& u2 = uv[index[i + 1]];
			const glm::vec2& u3 = uv[index[i + 2]];

			using vertex = std::array<int, 4>;

			vertex v1 {p1.x, p1.y, u1.x, u1.y};
			vertex v2 {p2.x, p2.y, u2.x, u2.y};
			vertex v3 {p3.x, p3.y, u3.x, u3.y};

			RasterPolygon(v1, v2, v3, [&](int x, int y, float u, float v)
			{
				func(x, y, u, v);
			});
		}
	}

// small software renderer, could take out and make its own class, https://www.youtube.com/watch?v=PahbNFypubE

	template<
		typename _v,
		typename _f1, typename _f2, typename _f3>
	void RasterTriangle(
		const _v* v0, const _v* v1, const _v* v2,
		_f1&& GetXY,
		_f2&& MakeSlope,
		_f3&& DrawScanline)
	{
		auto [x0, y0, x1, y1, x2, y2] = std::tuple_cat(GetXY(*v0), GetXY(*v1), GetXY(*v2));

		if (std::tie(y1, x1) < std::tie(y0, x0)) { std::swap(x0, x1); std::swap(y0, y1); std::swap(v0, v1); }
		if (std::tie(y2, x2) < std::tie(y0, x0)) { std::swap(x0, x2); std::swap(y0, y2); std::swap(v0, v2); }
		if (std::tie(y2, x2) < std::tie(y1, x1)) { std::swap(x1, x2); std::swap(y1, y2); std::swap(v1, v2); }

		if (y0 == y2) return; // 0 area

		bool shortside = (y1 - y0) * (x2 - x0) < (x1 - x0) * (y2 - y0);

		std::invoke_result_t<_f2, const _v&, const _v&, int> sides[2];
		sides[!shortside] = MakeSlope(*v0, *v2, y2 - y0);

		for (auto y = y0, endy = y0; /**/; ++y) // I wonder if this actually helps the inlinning of lambdas
		{
			if (y >= endy)
			{
				if (y >= y2) break;
				sides[shortside] = std::apply(MakeSlope, (y < y1) ? std::tuple(*v0, *v1, (endy = y1) - y0)
														: std::tuple(*v1, *v2, (endy = y2) - y1)); // assignment
			}

			DrawScanline(y, sides[0], sides[1]);
		}
	}

	struct Slope {
		
		float m_cur, m_step;

		Slope() = default;
		Slope(float begin, float end, int steps) {
			float inv_step = 1.0f / steps;
			m_cur = begin;
			m_step = (end - begin) * inv_step;
		}

		float get() { return m_cur; }
		void step() { m_cur += m_step; }
	};

	using vertex = std::array<int, 4>; // x, y, u, v

	template<
		typename _f>
	void RasterPolygon(
		const vertex& v0, const vertex& v1, const vertex& v2,
		_f&& PlotPoint)
	{
		using SlopeData = std::array<Slope, 3>; // x, u, v

		RasterTriangle(&v0, &v1, &v2,
			[](const vertex& vert)
			{
				return std::make_pair(vert[0], vert[1]);
			},
			[](const vertex& from, const vertex& to, int steps)
			{
				SlopeData slopes;
				slopes[0] = Slope(from[0], to[0], steps); // x
				slopes[1] = Slope(from[2], to[2], steps); // u
				slopes[2] = Slope(from[3], to[3], steps); // v

				return slopes;
			},
			[&](int y, SlopeData& left, SlopeData& right)
			{
				int x = left[0].get(), endx = right[0].get();

				Slope props[2]; // u, v inter scanline slopes
				props[0] = Slope(left[1].get(), right[1].get(), endx - x);
				props[1] = Slope(left[2].get(), right[2].get(), endx - x);

				for (; x < endx; ++x)
				{
					PlotPoint(x, y, props[0].get(), props[1].get());
					for (auto& slope : props) slope.step();
				}

				for (auto& slope : left)  slope.step();
				for (auto& slope : right) slope.step();
			}
		);
	}
};

IW_PLUGIN_SAND_END
