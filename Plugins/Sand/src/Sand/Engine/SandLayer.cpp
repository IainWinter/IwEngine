#include "iw/Sand/Engine/SandLayer.h"
#include "iw/log/logger.h"

#include "iw/physics/Collision/MeshCollider.h"

IW_PLUGIN_SAND_BEGIN

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

	m_world = new SandWorld(64 * m_cellSize, 64 * m_cellSize, m_cellSize);

	m_world->m_workers.push_back(new SandWorkerBuilder<SimpleSandWorker>());

	m_update = PushSystem<SandWorldUpdateSystem>(m_world);
	m_render = PushSystem<SandWorldRenderSystem>(m_world);
	
	m_update->SetCameraScale(m_cellsPerMeter, m_cellsPerMeter);

	if (int error = Layer::Initialize()) {
		return error;
	}

	LOG_INFO << "World chunk size: " << m_world->m_chunkWidth << ", " << m_world->m_chunkHeight;

	return 0;
}

void SandLayer::PreUpdate() {
	//int width  = m_render->GetSandTexture()->Width();
	//int height = m_render->GetSandTexture()->Height();

	//int fx = -width  / 2;
	//int fy = -height / 2;
	//int fx2 = fx + width;
	//int fy2 = fy + height;

	////m_render->SetCamera(fx, fy, fx2, fy2);

	int width  = m_render->m_fx2 - m_render->m_fx;
	int height = m_render->m_fy2 - m_render->m_fy;

	sP = Mouse::ClientPos() / m_world->m_scale + vec2(m_render->m_fx, -m_render->m_fy);
	sP.y() = height - sP.y();

	gP = vec2(int(sP.x() / gridSize), int(sP.y() / gridSize)) * gridSize;

	DrawWithMouse(m_render->m_fx, -m_render->m_fy, width, height);

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

void SandLayer::PasteTiles()
{
	Space->Query<iw::Transform, Tile>().Each([&](
		auto entity,
		Transform* transform,
		Tile* tile)
	{
		if (tile->NeedsScan)
		{
			tile->NeedsScan = false;

			tile->UpdatePolygon(m_update->m_sx, m_update->m_sy);

			MeshCollider2* collider = Space->FindEntity(tile).Find<MeshCollider2>();
			collider->SetPoints(tile->m_polygon);
			collider->SetTriangles(tile->m_index);

			Mesh* mesh = Space->FindEntity(tile).Find<Mesh>(); // tmep debug
			mesh->Data->SetBufferData(bName::POSITION, tile->m_polygon.size(), tile->m_polygon.data());
			mesh->Data->SetIndexData(                  tile->m_index  .size(), tile->m_index  .data());
		}

		// if actually moved

		if (!tile->IsStatic) {
			tile->Draw(transform, Renderer->ImmediateMode());
		}
		
		Cell cell;
		cell.Props = CellProperties::NONE_TILE;
		cell.Color = Color::From255(150, 150, 150);
		cell.Type = CellType::ROCK;

		tile->ForEachInWorld(transform, m_update->m_sx, m_update->m_sy, [&](
			int x, int y,
			unsigned data)
		{
			if (m_world->InBounds(x, y)) {
				m_world->SetCell(x, y, cell);
			}
		});

		tile->LastTransform = *transform;
	});
}

void SandLayer::RemoveTiles()
{
	Space->Query<iw::Transform, Tile>().Each([&](
		auto entity,
		Transform* transform,
		Tile* tile)
	{
		tile->ForEachInWorld(&tile->LastTransform, m_update->m_sx, m_update->m_sy, [&](
			int x, int y,
			unsigned data)
		{
			if (m_world->InBounds(x, y)) {
				m_world->SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
			}
		});
	});
}

Entity SandLayer::MakeTile(
	const std::string& sprite,
	bool isStatic,
	bool isSimulated)
{
	ref<Archetype> archetype = Space->CreateArchetype<Transform, Mesh, MeshCollider2, Tile>(); // Mesh is just temp for debug

	if (isSimulated) Space->AddComponent<Rigidbody>      (archetype);
	else             Space->AddComponent<CollisionObject>(archetype);
	
	Entity entity = Space->CreateEntity(archetype);

	Tile* tile = entity.Set<Tile>(Asset, "", isStatic);

	Transform*       transform = entity.Set<Transform>();
	MeshCollider2*   collider  = entity.Set<MeshCollider2>();
	CollisionObject* object    = isSimulated ? entity.Set<Rigidbody>() : entity.Set<CollisionObject>();

	entity.Set<Mesh>(tile->m_spriteMesh.MakeCopy()); // temp debug

	object->Collider = collider;
	object->SetTransform(transform);

	return entity;
}

void SandLayer::FillPolygon(
	const std::vector<glm::vec2>& polygon, 
	const std::vector<unsigned>&  index,
	const iw::Cell& cell)
{
	for (size_t i = 0; i < index.size(); i += 3) {
		glm::vec2 v0 = polygon[index[i    ]] * float(m_cellsPerMeter);
		glm::vec2 v1 = polygon[index[i + 1]] * float(m_cellsPerMeter);
		glm::vec2 v2 = polygon[index[i + 2]] * float(m_cellsPerMeter);
		
		glm::vec2 v01 = v1 - v0;
		glm::vec2 v02 = v2 - v0;

		int maxX = iw::max(v0.x, iw::max(v1.x, v2.x));
		int minX = iw::min(v0.x, iw::min(v1.x, v2.x));
		int maxY = iw::max(v0.y, iw::max(v1.y, v2.y));
		int minY = iw::min(v0.y, iw::min(v1.y, v2.y));

		for (int x = minX; x <= maxX; x++)
		{
			for (int y = minY; y <= maxY; y++)
			{
				glm::vec2 q(x - v0.x, y - v0.y);

				float s = (float)iw::cross_length(q, v02) / iw::cross_length(v01, v02);
				float t = (float)iw::cross_length(v01, q) / iw::cross_length(v01, v02);

				if (   s     >= 0 
					&& t     >= 0 
					&& s + t <= 1)
				{
					//iw::Cell c;

					//c.Type  = CellType::ROCK;
					//c.Color = Color::From255(100, 100, 100);
					//c.Style = CellStyle::RANDOM_STATIC;
					//c.StyleColor = Color(.05, .05, .05, 0);
					//c.StyleOffset = randf();

					m_world->SetCell(x, y, cell);
				}
			}
		}
	}
}

IW_PLUGIN_SAND_END
