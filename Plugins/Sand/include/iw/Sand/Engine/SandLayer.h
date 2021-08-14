#pragma once

#include "iw/engine/Layer.h"

#include "../Workers/SimpleSandWorker.h"
#include "SandUpdateSystem.h"
#include "SandRenderSystem.h"

#include "iw/graphics/software/renderer.h"
#include <unordered_map>

#include "iw/physics/Collision/MeshCollider.h"

IW_PLUGIN_SAND_BEGIN

using TileInfo = std::pair<Tile*, unsigned>; // tile, index

class SandLayer
	: public Layer
{
public:
	SandWorld* m_world;
	const int m_cellSize;
	const int m_cellsPerMeter;

	int gridSize = 16; // add m_ 
	glm::vec2 sP, gP; // sand pos, grid pos

private:
	SandWorldRenderSystem* m_render;
	SandWorldUpdateSystem* m_update;

	bool m_drawMouseGrid;

	using SpriteData = std::tuple<Tile*, unsigned*, unsigned>; // tile, colors, width
	using PixelData  = std::tuple<int, int, Tile*, unsigned*, unsigned>;  // x, y, tile, colors, index

	std::vector<Tile*> m_tilesThisFrame;
	std::vector<std::pair<SandChunk*, std::vector<PixelData>>> m_cellsThisFrame;

	// options for a fixed size
	bool m_initFixed;
	int m_worldWidth; // these are invalid if initFixed is false
	int m_worldHeight;

public:
	SandLayer(
		int cellSize,
		int cellsPerMeter,
		bool expandable = false,
		bool drawMouseGrid = false
	)
		: Layer("Sand")

		, m_cellSize(cellSize)
		, m_cellsPerMeter(cellsPerMeter)
		, m_drawMouseGrid(drawMouseGrid)

		, m_initFixed(false)
		, m_worldWidth (-1)
		, m_worldHeight(-1)

		, m_world(nullptr)
		, m_render(nullptr)
		, m_update(nullptr)
		, sP(0.f)
		, gP(0.f)
	{}

	SandLayer(
		int cellSize,
		int cellsPerMeter,
		int worldWidth,
		int worldHeight,
		bool drawMouseGrid = false
	)
		: Layer("Sand")

		, m_cellSize(cellSize)
		, m_cellsPerMeter(cellsPerMeter)
		, m_drawMouseGrid(drawMouseGrid)

		, m_initFixed(true)
		, m_worldWidth (worldWidth  / cellSize)
		, m_worldHeight(worldHeight / cellSize)

		, m_world(nullptr)
		, m_render(nullptr)
		, m_update(nullptr)
		, sP(0.f)
		, gP(0.f)
	{}

	IW_PLUGIN_SAND_API int  Initialize();
	IW_PLUGIN_SAND_API void PreUpdate();
	IW_PLUGIN_SAND_API void PostUpdate();

	IW_PLUGIN_SAND_API void DrawMouseGrid();
	IW_PLUGIN_SAND_API void DrawWithMouse(int fx, int fy, int width, int height);

	IW_PLUGIN_SAND_API bool On(MouseWheelEvent& e);

	IW_PLUGIN_SAND_API void PasteTiles();
	IW_PLUGIN_SAND_API void RemoveTiles();

	IW_PLUGIN_SAND_API void EjectPixel(Tile* tile, unsigned index);

	Mesh& GetSandMesh() {
		return m_render->GetSandMesh();
	}

	// returns half of the sizw of the same texture
	std::pair<int, int> GetSandTexSize2()
	{
		int width  = m_render->GetSandTexture()->Width() / 2;
		int height = m_render->GetSandTexture()->Height() / 2;

		return { width, height };
	}

	void SetCamera(
		float x,  float y,
		float xs, float ys)
	{
		auto [width, height] = GetSandTexSize2();

		//x = ceil(x * xs);
		//y = ceil(y * ys);

		m_render->SetCamera(x + -width, y + -height, x + width, y + height);
		m_update->SetCameraScale(xs, ys);
	}

	inline Entity MakeTile(
		const std::string& sprite,
		bool isSimulated = false)
	{
		return MakeTile(Asset->Load<iw::Texture>(sprite), isSimulated);
	}

	template<
		typename _collider = MeshCollider2,
		typename... _others>
	Entity MakeTile(
		ref<Texture>& sprite,
		bool isSimulated)
	{
		ref<Archetype> archetype = Space->CreateArchetype<Transform, Tile, _collider, _others...>();

		if (isSimulated) Space->AddComponent<Rigidbody>      (archetype);
		else             Space->AddComponent<CollisionObject>(archetype);
	
		Entity entity = Space->CreateEntity(archetype);

		Tile* tile = entity.Set<Tile>(sprite);
	
		Transform*       transform = entity.Set<Transform>();
		_collider*       collider  = entity.Set<_collider>();
		CollisionObject* object    = isSimulated ? entity.Set<Rigidbody>() : entity.Set<CollisionObject>();

		object->Collider = collider;
		object->SetTransform(transform);

		if (isSimulated) Physics->AddRigidbody((Rigidbody*)object);
		else             Physics->AddCollisionObject(object);

		return entity;
	}

	void ForEachInPolygon(
		const std::vector<glm::vec2>& polygon,
		const std::vector<glm::vec2>& uv,
		const std::vector<unsigned>&  index,
		std::function<void(int, int, float, float, int)> func)
	{
		for (size_t i = 0; i < index.size(); i += 3) {
			const glm::vec2& p1 = polygon[index[i]];
			const glm::vec2& p2 = polygon[index[i + 1]];
			const glm::vec2& p3 = polygon[index[i + 2]];
		
			const glm::vec2& u1 = uv[index[i]];
			const glm::vec2& u2 = uv[index[i + 1]];
			const glm::vec2& u3 = uv[index[i + 2]];

			using vertex = software_renderer::vertex;

			vertex v1 {p1.x, p1.y, u1.x, u1.y};
			vertex v2 {p2.x, p2.y, u2.x, u2.y};
			vertex v3 {p3.x, p3.y, u3.x, u3.y};

			software_renderer
			::RasterPolygon(v1, v2, v3, [&](int x, int y, float u, float v)
			{
				func(x, y, u, v, i / 3);
			});
		}
	}

	void ForEachInLine(
		float x,  float y,
		float x1, float y1,
		std::function<bool(int, int)> func)
	{
		using vertex = std::array<int, 2>;

		vertex v0 { x,  y };
		vertex v1 { x1, y1};

		software_renderer
		::RasterLine(v0, v1,
			[&](int x, int y)
			{
				return func(x, y);
			}
		);
	}

	// Rendering tiles into world

private:

	template<typename _f1, typename _f2>
	using PixelDataf = std::invoke_result_t<_f2, std::invoke_result_t<_f1, Tile*>, int, int, float, float, int>;

	template<typename _f1, typename _f2>
	using PixelDataGridf = std::vector<std::vector<PixelDataf<_f1, _f2>>>;

	using GridLocation = std::tuple<int, int, int, int>;

	template<
		typename _f1,
		typename _f2,
		typename _f3>
	std::vector<std::pair<SandChunk*, std::vector<PixelDataf<_f1, _f2>>>>
	RasterTilesIntoSandWorld(
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

		unordered_map<pair<int, int>, vector<vector<PixelData>>, pair_hash> grid;

		for (auto& [location, pixels] : cache)
		{
			auto& [x_min, y_min, x_size, y_size] = location;
		
			for (int x = 0; x < x_size; x++)
			for (int y = 0; y < y_size; y++)
			{
				grid[{x + x_min, y + y_min}].emplace_back(move(pixels[x + y * x_size]));
			}
		}

		Task->foreach(
			grid,
			[&](auto& element) 
			{
				auto& [location, gridPixels] = element;
				return make_pair(m_world->GetChunkL(location), &gridPixels);
			},
			[&](int index, auto& chunkPixels) 
			{
				auto& [chunk, gridPixels] = chunkPixels;

				for (vector<PixelData>& pixels : *gridPixels)
				for (PixelData& data : pixels)
				{
					WritePixelData(chunk, data);
				}
			}
		);

		vector<std::pair<SandChunk*, vector<PixelData>>> outPixels;

		for (auto& [location, gridPixels] : grid)
		{
			SandChunk* chunk = m_world->GetChunkL(location);
			for (vector<PixelData>& pixels : gridPixels)
			{
				outPixels.emplace_back(chunk, move(pixels));
			}
		}

		return outPixels;
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
		ForEachInPolygon(polygon, tile->m_uv, tile->m_index, [&](int x, int y, float u, float v, int tri)
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

			pixels[px + py * x_size].emplace_back(GetPixelData(pre, x, y, u, v, tri));
		});

		return {
			{ cx_min, cy_min, x_size, y_size },
			pixels
		};
	}
};

IW_PLUGIN_SAND_END
