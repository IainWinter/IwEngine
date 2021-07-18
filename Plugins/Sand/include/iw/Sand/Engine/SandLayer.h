#pragma once

#include "iw/engine/Layer.h"

#include "../Workers/SimpleSandWorker.h"
#include "SandUpdateSystem.h"
#include "SandRenderSystem.h"

#include "iw/graphics/software/renderer.h"
#include <unordered_map>

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

	std::vector<Tile*> m_tilesThisFrame;

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

	IW_PLUGIN_SAND_API void PasteTiles (const std::vector<Tile*>& tiles);
	IW_PLUGIN_SAND_API void RemoveTiles(const std::vector<Tile*>& tiles);

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

	void ForEachInPolygon(
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

			software_renderer::RasterPolygon(v1, v2, v3, [&](int x, int y, float u, float v)
			{
				func(x, y, u, v);
			});
		}
	}

	// Rendering tiles into world

private:

	template<typename _f1, typename _f2>
	using PixelDataf = std::invoke_result_t<_f2, std::invoke_result_t<_f1, Tile*>, int, int, int, int>;

	template<typename _f1, typename _f2>
	using PixelDataGridf = std::vector<std::vector<PixelDataf<_f1, _f2>>>;

	using GridLocation = std::tuple<int, int, int, int>;

	template<
		typename _f1,
		typename _f2,
		typename _f3>
	void RasterTilesIntoSandWorld(
		const std::vector<Tile*>& tiles,
		_f1&& GetPixelDataPre,
		_f2&& GetPixelData,
		_f3&& WritePixelData)
	{
		using namespace std;
	
		vector<pair<GridLocation, PixelDataGridf<_f1, _f2>>> cache;
		cache.resize(tiles.size());

		Task->foreach(
			tiles,
			[](Tile* tile) { return tile; },
			[&](int index, Tile* tile) 
			{
				cache[index] = RasterTileIntoGrid(tile, GetPixelDataPre, GetPixelData);
			}
		);

		using PixelData = PixelDataf<_f1, _f2>;

		unordered_map<pair<int, int>, vector<vector<PixelData>*>, pair_hash> grid;

		for (auto& [location, pixels] : cache)
		{
			auto& [x_min, y_min, x_size, y_size] = location;
		
			for (int x = 0; x < x_size; x++)
			for (int y = 0; y < y_size; y++)
			{
				grid[{x + x_min, y + y_min}].push_back(&pixels[x + y * x_size]);
			}
		}

		Task->foreach(
			grid,
			[&](const auto& element) 
			{
				auto& [location, gridPixels] = element;
				return make_pair(m_world->GetChunkL(location), &gridPixels);
			},
			[&](int index, auto& chunkPixels) 
			{
				auto& [chunk, gridPixels] = chunkPixels;

				for (vector<PixelData>* pixels : *gridPixels)
				for (PixelData& data : *pixels)
				{
					WritePixelData(chunk, data);
				}
			}
		);
	}

	template<
		typename _f1, typename _f2>
	std::pair<GridLocation, PixelDataGridf<_f1, _f2>>
	RasterTileIntoGrid(
		Tile* tile,
		_f1&& GetPixelDataPre,
		_f2&& GetPixelData)
	{
		auto [min, max] = TransformBounds(tile->m_bounds, &tile->LastTransform);

		int x_min = floor(min.x),
		    y_min = floor(min.y),
		    x_max = ceil (max.x), 
		    y_max = ceil (max.y);

		auto c_min = m_world->GetChunkLocation(x_min, y_min);
		auto c_max = m_world->GetChunkLocation(x_max, y_max);

		int cx_min = c_min.first,
		    cy_min = c_min.second,
		    cx_max = c_max.first,
		    cy_max = c_max.second;

		int x_size = cx_max - cx_min + 1;
		int y_size = cy_max - cy_min + 1;

		auto pre = GetPixelDataPre(tile);

		PixelDataGridf<_f1, _f2> pixels;
		pixels.resize(x_size * y_size);

		std::vector<glm::vec2> polygon = tile->m_polygon;
		TransformPolygon(polygon, &tile->LastTransform);
		ForEachInPolygon(polygon, tile->m_uv, tile->m_index, [&](int x, int y, int u, int v)
		{
			auto [px, py] = m_world->GetChunkLocation(x, y);
			px -= cx_min;
			py -= cy_min;

			if (    px < 0 
				|| py < 0 
				|| px >= x_size 
				|| py >= y_size) // shouldn't need
			{
				return;
			}

			pixels[px + py * x_size].emplace_back(GetPixelData(pre, x, y, u, v));
		});

		return {
			{ cx_min, cy_min, x_size, y_size },
			pixels
		};
	}
};

IW_PLUGIN_SAND_END
