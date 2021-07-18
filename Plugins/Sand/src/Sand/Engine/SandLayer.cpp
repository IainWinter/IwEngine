#include "iw/Sand/Engine/SandLayer.h"
#include "iw/log/logger.h"

#include "iw/physics/Collision/MeshCollider.h"
#include "iw/common/algos/polygon2.h"

IW_PLUGIN_SAND_BEGIN

void CorrectCellInfo(SandChunk* chunk, size_t index, int x, int y, void* data) {
	// set location if its not set, this is a hack

	Cell& cell = *(Cell*)data;

	if (cell.x == 0 && cell.y == 0) {
		cell.x = chunk->m_x + x;
		cell.y = chunk->m_y + y;
	}

	// set location everytime it changed whole number

	cell.x = float(cell.x - int(cell.x)) + x + chunk->m_x;
	cell.y = float(cell.y - int(cell.y)) + y + chunk->m_y;

	char notEmpty = cell.Type  != CellType::EMPTY;
	
	chunk->SetCell(index, cell.Color.to32(), SandField::COLOR);
	chunk->SetCell(index, notEmpty,          SandField::SOLID);

	// was for collision for platformer game

	//bool collision = cell.Props == CellProperties::NONE && notEmpty;

	//chunk->SetCell<bool>(index, collision, SandField::COLLISION);
}

int SandLayer::Initialize() {
	Cell _EMPTY = {
		CellType::EMPTY,
		CellProperties::NONE,
		CellStyle::NONE,
		Color::From255(0, 0, 0, 0)
	};

	Cell _SAND = {
		CellType::SAND,
		/*CellProperties::MOVE_FORCE | */CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE,
		CellStyle::RANDOM_STATIC,
		Color::From255(235, 200, 175),
		Color::From255(25, 25, 25, 0)
	};

	Cell _WATER = {
		CellType::WATER,
		/*CellProperties::MOVE_FORCE | */CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE,
		CellStyle::SHIMMER,
		Color::From255(175, 200, 235),
		Color::From255(25, 25, 25, 0)
	};

	Cell _ROCK = {
		CellType::ROCK,
		CellProperties::NONE,
		CellStyle::NONE,
		Color::From255(200, 200, 200)
	};

	Cell _STONE = {
		CellType::STONE,
		CellProperties::MOVE_DOWN | CellProperties::MOVE_FORCE,
		CellStyle::RANDOM_STATIC,
		Color::From255(200, 200, 200),
		Color::From255(10, 10, 10, 0),
	};

	Cell _WOOD = {
		CellType::WOOD,
		CellProperties::NONE,
		CellStyle::RANDOM_STATIC,
		Color::From255(171, 121, 56),
		Color::From255(15, 10, 10, 0)
	};

	Cell _FIRE = {
		CellType::FIRE,
		/*CellProperties::MOVE_FORCE | */CellProperties::BURN | CellProperties::MOVE_RANDOM,
		CellStyle::NONE,
		Color::From255(255, 98, 0) // figure out how to blend colors properly, this makes it magenta because red gets overflown??? but still happens with 200
	};

	Cell _SMOKE = {
		CellType::SMOKE,
		/*CellProperties::MOVE_FORCE | */CellProperties::MOVE_RANDOM,
		CellStyle::RANDOM_STATIC,
		Color::From255(100, 100, 100),
		Color::From255(25, 25, 25, 0)
	};

	Cell _BELT = {
		CellType::BELT,
		CellProperties::CONVEYOR,
		CellStyle::SHIMMER,
		Color::From255(201, 129, 56),
		Color::From255(25, 25, 25, 0)
	};

	Cell::SetDefault(CellType::EMPTY, _EMPTY);
	Cell::SetDefault(CellType::SAND,  _SAND);
	Cell::SetDefault(CellType::WATER, _WATER);
	Cell::SetDefault(CellType::ROCK,  _ROCK);
	Cell::SetDefault(CellType::STONE, _STONE);
	Cell::SetDefault(CellType::WOOD,  _WOOD);
	Cell::SetDefault(CellType::FIRE,  _FIRE);
	Cell::SetDefault(CellType::SMOKE, _SMOKE);
	Cell::SetDefault(CellType::BELT,  _BELT);

	int chunkSize = 100;

	m_world = new SandWorld(
		m_cellSize * chunkSize,
		m_cellSize * chunkSize,
		m_cellSize
	);
	
	m_world->m_workers.push_back(new SandWorkerBuilder<SimpleSandWorker>());

	// this code SUCKS, the order has to be the same as the order in the enum

	//REGISTER_FIELD(Cell, true, CELL); somehow need something like this in an included file

	m_world->AddField<Cell>(true, CorrectCellInfo); // cells
	m_world->AddField<unsigned char>(false); // if solid, for IsEmpty / tiles
	m_world->AddField<unsigned int>(false); // color for drawing / tiles

	//m_world->CreatedChunkCallback = [&](SandChunk* chunk) 
	//{
	//	ref<Texture> color = REF<Texture>(chunk->m_width, chunk->m_height, TEX_2D, RGBA);
	//	ref<Texture> solid = REF<Texture>(chunk->m_width, chunk->m_height, TEX_2D, ALPHA);
	//	color->m_filter = NEAREST;
	//	solid->m_filter = NEAREST;

	//	color->m_colors = chunk->GetField(SandField::COLOR).GetCells<unsigned char>();
	//	solid->m_colors = chunk->GetField(SandField::SOLID).GetCells<unsigned char>();

	//	ref<RenderTarget> target = REF<RenderTarget>();
	//	target->AddTexture(color);
	//	target->AddTexture(solid);

	//	m_chunkTextures[m_world->GetChunkLocation(chunk)] = target;
	//};

	//m_world->RemovedChunkCallback = [&](SandChunk* chunk) {
	//	m_chunkTextures.erase(m_world->GetChunkLocation(chunk));
	//};
	// 
	//m_tiles = grid2<Tile*>(chunkSize / m_cellsPerMeter);

	m_update = PushSystem<SandWorldUpdateSystem>(m_world);
	m_render = PushSystem<SandWorldRenderSystem>(m_world);
	
	m_update->SetCameraScale(m_cellsPerMeter, m_cellsPerMeter);

	if (int error = Layer::Initialize()) {
		return error;
	}

	LOG_INFO << "Sand world chunk size: " << m_world->m_chunkWidth << ", " << m_world->m_chunkHeight;

	return 0;
}

void SandLayer::PreUpdate() {
	int width  = m_render->m_fx2 - m_render->m_fx;
	int height = m_render->m_fy2 - m_render->m_fy;

	sP = Mouse::ClientPos() / m_world->m_scale + vec2(m_render->m_fx, -m_render->m_fy);
	sP.y() = height - sP.y();

	gP = vec2(int(sP.x() / gridSize), int(sP.y() / gridSize)) * gridSize;

	DrawWithMouse(m_render->m_fx, -m_render->m_fy, width, height);

	m_tilesThisFrame.clear();
	Space->Query<Transform, Tile>().Each([&](
		auto entity,
		Transform* transform,
		Tile* tile)
	{
		m_tilesThisFrame.push_back(tile);

		if (tile->NeedsScan)
		{
			tile->NeedsScan = false;
			tile->UpdatePolygon();

			MeshCollider2* collider = Space->FindEntity(tile).Find<MeshCollider2>();
			collider->SetPoints(tile->m_polygon);
			collider->SetTriangles(tile->m_index);
		}

		tile->LastTransform = *transform;
	});

	PasteTiles(m_tilesThisFrame);
}

void SandLayer::PostUpdate()
{
	if (m_drawMouseGrid) {
		DrawMouseGrid();
	}

	RemoveTiles(m_tilesThisFrame);
}

void SandLayer::DrawMouseGrid() {
	unsigned* sandColor = (unsigned*)m_render->GetSandTexture()->Colors();
	int width  = m_render->GetSandTexture()->Width();
	int height = m_render->GetSandTexture()->Height();

	auto draw = [&](size_t x, size_t y) {
		size_t index = x + y * width;
		if (index * height) return;
		sandColor[index] = Color(1).to32();
	};

	float x = gP.x();
	float y = gP.y();

	for (int i = 1; i < gridSize; i++) draw(x + i,        y); 
	for (int i = 1; i < gridSize; i++) draw(x + i,        y + gridSize);
	for (int i = 1; i < gridSize; i++) draw(x,            y + i);
	for (int i = 1; i < gridSize; i++) draw(x + gridSize, y + i);

	m_render->GetSandTexture()->Update(Renderer->Device);
}

void SandLayer::DrawWithMouse(int fx, int fy, int width, int height) {
	if (Mouse::ButtonDown(LMOUSE)) {
		CellType placeMe = CellType::EMPTY;

			 if (Keyboard::KeyDown(S)) placeMe = CellType::SAND;
		else if (Keyboard::KeyDown(W)) placeMe = CellType::WATER;
		else if (Keyboard::KeyDown(R)) placeMe = CellType::ROCK;
		else if (Keyboard::KeyDown(T)) placeMe = CellType::STONE;
		else if (Keyboard::KeyDown(F)) placeMe = CellType::FIRE;
		else if (Keyboard::KeyDown(M)) placeMe = CellType::SMOKE;
		else if (Keyboard::KeyDown(O)) placeMe = CellType::WOOD;
		else if (Keyboard::KeyDown(B)) placeMe = CellType::BELT;

		if (    placeMe == CellType::EMPTY
			&& !Keyboard::KeyDown(C)) 
		{
			return;
		}

		for (int x = gP.x(); x < gP.x() + gridSize; x++)
		for (int y = gP.y(); y < gP.y() + gridSize; y++) {
			if (!m_world->InBounds(x, y))
				continue;

			Cell cell = Cell::GetDefault(placeMe);
			cell.StyleOffset = randf();

			m_world->SetCell(x, y, cell);
		}
	}
}

bool SandLayer::On(MouseWheelEvent& e) {
	gridSize = clamp<int>(gridSize + e.Delta, 1, 64);
	return false;
}

// This works for now but if preformance is a problem could add a bulk submit feature to chunk to eak a little more out

void SandLayer::PasteTiles(
	const std::vector<Tile*>& tiles)
{
	using SpriteData = std::pair<unsigned*, unsigned>; // color, width
	using PixelData  = std::tuple<int, int, unsigned>; // x, y, color

	RasterTilesIntoSandWorld(tiles,
		[](Tile* tile) {
			auto& sprite = tile->m_sprite;
			return SpriteData((unsigned*)sprite->Colors(), sprite->Width());
		},
		[](SpriteData& sprite, int x, int y, int u, int v) {
			auto& [colors, width] = sprite;
			return PixelData(x, y, colors[u + v * width]);
		},
		[](SandChunk* chunk, PixelData& data)
		{
			auto& [x, y, color] = data;
			chunk->SetCell_unsafe(x, y, color, SandField::COLOR);
			chunk->SetCell_unsafe(x, y, true,  SandField::SOLID);
		}
	);
}

void SandLayer::RemoveTiles(
	const std::vector<Tile*>& tiles)
{
	using PixelData = std::pair<int, int>; // x, y

	RasterTilesIntoSandWorld(tiles,
		[](Tile* tile) {
			return tile; // little funky dont need this but there is no way to not have it lol
		},
		[](Tile* tile, int x, int y, int u, int v) {
			return PixelData(x, y);
		},
		[](SandChunk* chunk, PixelData& data)
		{
			auto& [x, y] = data;
			chunk->SetCell_unsafe(x, y, 0u,    SandField::COLOR);
			chunk->SetCell_unsafe(x, y, false, SandField::SOLID);
		}
	);
}

Entity SandLayer::MakeTile(
	ref<Texture>& sprite,
	bool isSimulated)
{
	ref<Archetype> archetype = Space->CreateArchetype<Transform, MeshCollider2, Tile>();

	if (isSimulated) Space->AddComponent<Rigidbody>      (archetype);
	else             Space->AddComponent<CollisionObject>(archetype);
	
	Entity entity = Space->CreateEntity(archetype);

	Tile* tile = entity.Set<Tile>(sprite);
	
	Transform*       transform = entity.Set<Transform>();
	MeshCollider2*   collider  = entity.Set<MeshCollider2>();
	CollisionObject* object    = isSimulated ? entity.Set<Rigidbody>() : entity.Set<CollisionObject>();

	object->Collider = collider;
	object->SetTransform(transform);

	if (isSimulated) Physics->AddRigidbody((Rigidbody*)object);
	else             Physics->AddCollisionObject(object);

	return entity;
}

IW_PLUGIN_SAND_END
