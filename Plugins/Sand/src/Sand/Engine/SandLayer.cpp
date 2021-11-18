#include "iw/Sand/Engine/SandLayer.h"
#include "iw/log/logger.h"
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

	//bool collision = cell.Props == CellProp::NONE && notEmpty;

	//chunk->SetCell<bool>(index, collision, SandField::COLLISION);
}

int SandLayer::Initialize() {
	Cell _EMPTY = {
		CellType::EMPTY,
		CellProp::NONE,
		CellStyle::NONE,
		Color::From255(0, 0, 0, 0)
	};

	Cell _SAND = {
		CellType::SAND,
		CellProp::MOVE_FORCE | CellProp::MOVE_DOWN | CellProp::MOVE_DOWN_SIDE,
		CellStyle::RANDOM_STATIC,
		Color::From255(235, 200, 175),
		Color::From255(25, 25, 25, 0)
	};

	Cell _WATER = {
		CellType::WATER,
		CellProp::MOVE_FORCE | CellProp::MOVE_DOWN | CellProp::MOVE_SIDE,
		CellStyle::SHIMMER,
		Color::From255(175, 200, 235),
		Color::From255(25, 25, 25, 0)
	};

	Cell _ROCK = {
		CellType::ROCK,
		CellProp::NONE,
		CellStyle::NONE,
		Color::From255(200, 200, 200)
	};

	Cell _STONE = {
		CellType::STONE,
		CellProp::MOVE_DOWN | CellProp::MOVE_FORCE,
		CellStyle::RANDOM_STATIC,
		Color::From255(200, 200, 200),
		Color::From255(10, 10, 10, 0),
	};

	Cell _WOOD = {
		CellType::WOOD,
		CellProp::NONE,
		CellStyle::RANDOM_STATIC,
		Color::From255(171, 121, 56),
		Color::From255(15, 10, 10, 0)
	};

	Cell _FIRE = {
		CellType::FIRE,
		/*CellProp::MOVE_FORCE | */CellProp::BURN | CellProp::MOVE_RANDOM,
		CellStyle::NONE,
		Color::From255(255, 98, 0) // figure out how to blend colors properly, this makes it magenta because red gets overflown??? but still happens with 200
	};

	Cell _SMOKE = {
		CellType::SMOKE,
		/*CellProp::MOVE_FORCE | */CellProp::MOVE_RANDOM,
		CellStyle::RANDOM_STATIC,
		Color::From255(100, 100, 100),
		Color::From255(25, 25, 25, 0)
	};

	Cell _BELT = {
		CellType::BELT,
		CellProp::CONVEYOR,
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

	if (m_initFixed)
	{
		m_world = new SandWorld(
			m_worldWidth,   m_worldHeight,
			m_worldChunksX, m_worldChunksY,
			m_cellSize
		);
	}

	else {
		int chunkSize = 100;
		
		m_world = new SandWorld(
			m_cellSize * chunkSize,
			m_cellSize * chunkSize,
			m_cellSize
		);
	}

	m_world->m_workers.push_back(new SandWorkerBuilder<SimpleSandWorker>());

	// this code SUCKS, the order has to be the same as the order in the enum

	//REGISTER_FIELD(Cell, true, CELL); somehow need something like this in an included file

	m_world->AddField<Cell>(true, CorrectCellInfo); // cells
	m_world->AddField<unsigned char>(false); // if solid, for IsEmpty / tiles
	m_world->AddField<unsigned int>(false);  // color for drawing / tiles
	m_world->AddField<TileInfo>(false); // Tile* and sprite index for ejecting pixels

	m_world->RemovedChunkCallback = [&](SandChunk* chunk) {
		for (int i = 0; i < m_cellsThisFrame.size(); i++)
		{
			if (chunk == m_cellsThisFrame.at(i).first)
			{
				LOG_INFO << "Removed chunk " << chunk;
				m_cellsThisFrame.erase(m_cellsThisFrame.begin() + i);
				i--;
				//break;
			}
		}
	};

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

	m_render = PushSystem<SandWorldRenderSystem>(m_world, m_worldWidth, m_worldHeight); // trying to render before update for cells with lifes under deltatime
	m_update = PushSystem<SandWorldUpdateSystem>(m_world);
	
	m_update->SetCameraScale(m_cellsPerMeter, m_cellsPerMeter);

	LOG_INFO << "Sand world chunk size: " << m_world->m_chunkWidth << ", " << m_world->m_chunkHeight;

	return Layer::Initialize();
}

void SandLayer::Destroy()
{
	delete m_world;
	Layer::Destroy();
}

void SandLayer::PreUpdate() {
	int width  = m_render->m_fx2 - m_render->m_fx;
	int height = m_render->m_fy2 - m_render->m_fy;

	sP = to_glm(Mouse::ClientPos()) / (float)m_world->m_scale + glm::vec2(m_render->m_fx, -m_render->m_fy);
	sP.y = height - sP.y;

	gP = glm::vec2(int(sP.x / gridSize), int(sP.y / gridSize)) * (float)gridSize;

	if (m_drawMouseGrid)
	{
		DrawWithMouse(m_render->m_fx, -m_render->m_fy, width, height);
	}

	PasteTiles();
}

void SandLayer::PostUpdate()
{
	if (m_drawMouseGrid) {
		DrawMouseGrid();
	}

	RemoveTiles();
}

void SandLayer::DrawMouseGrid() {
	unsigned* sandColor = (unsigned*)m_render->GetSandTexture()->Colors();
	int width  = m_render->GetSandTexture()->Width();
	int height = m_render->GetSandTexture()->Height();

	auto draw = [&](int x, int y) 
	{
		int index = x + y * width;
		if (index * height) return;
		sandColor[index] = Color(1).to32();
	};

	float x = gP.x;
	float y = gP.y;

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

		int gx = (int)gP.x;
		int gy = (int)gP.y;

		for (int x = gx; x < gx + gridSize; x++)
		for (int y = gy; y < gy + gridSize; y++) 
		{
			if (!m_world->InBounds(x, y))
			{
				continue;
			}

			Cell cell = Cell::GetDefault(placeMe);
			cell.StyleOffset = randfs();

			m_world->SetCell(x, y, cell);
		}
	}
}

bool SandLayer::On(MouseWheelEvent& e) {
	gridSize = clamp<int>(gridSize + e.Delta, 1, 64);
	return false;
}

void SandLayer::PasteTiles() 
{
	m_tilesThisFrame.clear();
	m_cellsThisFrame.clear();

	Space->Query<Transform, Tile>().Each([&](
		EntityHandle entity,
		Transform* transform,
		Tile* tile)
	{
		if (tile->m_sandLayerIndex != m_sandLayerIndex) return;

		m_tilesThisFrame.push_back(tile);

		if (tile->NeedsScan)
		{
			tile->NeedsScan = false;

			MeshCollider2* collider = Space->FindComponent<MeshCollider2>(entity);
			if (collider) {
				tile->UpdateColliderPolygon();
				collider->Points = tile->m_collider;
				collider->Triangles = tile->m_colliderIndex;
			}
		}

		tile->LastTransform = *transform;
	});

	m_cellsThisFrame = RasterTilesIntoSandWorld(m_tilesThisFrame,
		[](Tile* tile)
		{
			return SpriteData(tile, tile->m_sprite.Colors32(), tile->m_sprite.Width());
		},
		[](SpriteData& sprite, int x, int y, float u, float v, int tri)
		{
			auto& [tile, colors, width] = sprite;
			unsigned index = (int)floor(u) + (int)floor(v) * width;

			return PixelData(x, y, tile, colors, index);
		},
		[&](SandChunk* chunk, PixelData& data)
		{
			auto& [x, y, tile, colors, index] = data;
			
			if (index >= tile->m_sprite.ColorCount32()) // shouldn't need this! see what opengl does?
			{
				//LOG_TRACE << "UV outside of tile sprite on paste!";
				return;
			}

			if (   !chunk 
				|| !chunk->IsEmpty(x, y)
				|| tile->State(index) != Tile::FILLED)
			{
				return;
			}
				
			chunk->SetCell_unsafe(x, y, colors[index],            SandField::COLOR);
			chunk->SetCell_unsafe(x, y, true,                     SandField::SOLID);
			chunk->SetCell_unsafe(x, y, TileInfo { tile, index }, SandField::TILE_INFO);
		}
	);
}

void SandLayer::RemoveTiles()
{
	for (auto& [chunk, pixels] : m_cellsThisFrame)
	{
		if (!chunk)
		{
			continue;
		}

		for (auto& [x, y, tile, colors, index] : pixels)
		{
			if (index >= tile->m_sprite.ColorCount32()) // shouldn't need this! see what opengl does?
			{
				//LOG_TRACE << "UV outside of tile sprite on remove!";
				continue;
			}

			size_t cindex = chunk->GetIndex(x, y);
			TileInfo& info = chunk->GetCell<TileInfo>(cindex, SandField::TILE_INFO);

			if (   tile != info.tile
				|| tile->State(index) != Tile::FILLED)
			{
				continue;
			}

			info.tile = nullptr;
			info.index = 0u;

			chunk->SetCell_unsafe(cindex, 0u,    SandField::COLOR);
			chunk->SetCell_unsafe(cindex, false, SandField::SOLID);
		}
	}

	for (iw::Tile* tile : m_tilesThisFrame)
	{
		tile->m_justRemovedCells.clear();
	}
}

void SandLayer::EjectPixel(
	Tile* tile,
	unsigned index)
{
	// cant scale transform if removing pixels like this
	// to fix, maybe could remove pixels in a scaled square around index?

	// place ejected pixel into world

	tile->RemovePixel(index);
}

Entity SandLayer::SplitTile(
	Entity& entity, 
	std::vector<int> indices)
{
	if (indices.size() <= 2)
	{
		return Entity();
	}

	int minX =  INT_MAX;
	int minY =  INT_MAX;
	int maxX = -INT_MAX;
	int maxY = -INT_MAX;

	iw::Tile* tile = entity.Find<iw::Tile>();

	for (int& i : indices)
	{
		auto [x, y] = iw::xy(i, (int)tile->m_sprite.m_width);

		if (x < minX) minX = x;
		if (y < minY) minY = y;
		if (x > maxX) maxX = x;
		if (y > maxY) maxY = y;
	}

	ref<Texture> texture = REF<Texture>(maxX - minX + 1, maxY - minY + 1);
	texture->CreateColors();

	for (int& i : indices)
	{
		auto [x, y] = iw::xy(i, (int)tile->m_sprite.m_width);
		
		int it = (x - minX) + (y - minY) * texture->m_width;

		LOG_INFO << "Copying from " << i << " to " << it;

		if (it > texture->ColorCount())
		{
			continue;
		}

		texture->Colors32()[it] = tile->m_sprite.Colors32()[i];
	}

	Entity split = MakeTile(*texture, true);

	// give split the pos and rot of the orignal peice

	iw::Transform* transform = entity.Find<iw::Transform>();
	iw::Rigidbody* rigidbody = entity.Find<iw::Rigidbody>();

	glm::vec2 midOld = entity.Find<iw::Tile>()->m_sprite.Dimensions() / 2.f;
	glm::vec2 midNew = split .Find<iw::Tile>()->m_sprite.Dimensions() / 2.f;
	glm::vec2 offset = iw::TransformPoint<iw::d2>(
			glm::vec2(
				maxX - floor(midOld.x) - floor(midNew.x),
				maxY - floor(midOld.y) - floor(midNew.y)
			),
			&transform->ScaleAndRotation()
		);

	iw::Transform* splitTran = split.Find<iw::Transform>();
	iw::Rigidbody* splitBody = split.Find<iw::Rigidbody>();

	glm::vec3 o3 = glm::vec3(offset, 0.f);

	*splitTran = *transform;
	splitTran->Position += o3;
	splitBody->SetTransform(splitTran);

	// adjust masses to ratio of pixels

	float mass = rigidbody->Mass();
	float massRatio = indices.size() / (float)tile->m_currentCells.size();

	float splitMass = mass * massRatio;

	rigidbody->SetMass(mass - splitMass);
	splitBody->SetMass(splitMass);

	// not required for position

	splitBody->Velocity = o3; // could add vel of projectile
	splitBody->AngularVelocity.z = iw::randf();

	return split;
}

IW_PLUGIN_SAND_END
