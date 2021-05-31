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
		CellProperties::MOVE_FORCE | CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE,
		CellStyle::RANDOM_STATIC,
		Color::From255(235, 200, 175),
		Color::From255(25, 25, 25, 0)
	};

	Cell _WATER = {
		CellType::WATER,
		CellProperties::MOVE_FORCE | CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE,
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
		CellProperties::MOVE_FORCE | CellProperties::BURN | CellProperties::MOVE_RANDOM,
		CellStyle::NONE,
		Color::From255(255, 98, 0) // figure out how to blend colors properly, this makes it magenta because red gets overflown??? but still happens with 200
	};

	Cell _SMOKE = {
		CellType::SMOKE,
		CellProperties::MOVE_FORCE | CellProperties::MOVE_RANDOM,
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

	m_world = new SandWorld(256, 256, m_cellScale);

	m_world->m_workers.push_back(new SandWorkerBuilder<SimpleSandWorker>());

	m_update = PushSystem<SandWorldUpdateSystem>(m_world);
	m_render = PushSystem<SandWorldRenderSystem>(m_world);
	
	if (int error = Layer::Initialize()) {
		return error;
	}

	LOG_INFO << "World chunk size: " << m_world->m_chunkWidth << ", " << m_world->m_chunkHeight;

	return 0;
}

void SandLayer::PreUpdate() {
	int width  = m_render->GetSandTexture()->Width();
	int height = m_render->GetSandTexture()->Height();

	int fx = -width  / 2;
	int fy = -height / 2;
	int fx2 = fx + width;
	int fy2 = fy + height;

	m_render->SetCamera(fx, fy, fx2, fy2);

	sP = Mouse::ClientPos() / m_world->m_scale + vec2(fx, -fy);
	sP.y() = height - sP.y();

	gP = vec2(int(sP.x() / gridSize), int(sP.y() / gridSize)) * gridSize;

	DrawWithMouse(fx, fy, width, height);
}

void SandLayer::PostUpdate()
{
	if (m_drawMouseGrid) {
		DrawMouseGrid();
	}

	//glm::vec3 aspect = glm::vec3(float(Renderer->Height()) / Renderer->Width(), 1, 1);

	//Renderer->BeginScene(MainScene);
	//	Renderer->DrawMesh(Transform(glm::vec3(0), aspect), m_render->GetSandMesh());
	//Renderer->EndScene();
}

void SandLayer::DrawMouseGrid() {
	unsigned* sandColor = (unsigned*)m_render->GetSandTexture()->Colors();
	int width = m_render->GetSandTexture()->Width();

	auto draw = [&](int x, int y) {
		sandColor[x + y * width] = Color(1).to32();
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
		else if (Keyboard::KeyDown(C)) placeMe = CellType::BELT;

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

Entity SandLayer::MakeTile(
	const std::string& sprite,
	bool isStatic,
	bool isSimulated)
{
	ref<Archetype> archetype = Space->CreateArchetype<Transform, Mesh, MeshCollider, Tile>(); // Mesh is just temp for debug

	if (isSimulated) Space->AddComponent<Rigidbody>      (archetype);
	else             Space->AddComponent<CollisionObject>(archetype);
	
	Entity entity = Space->CreateEntity(archetype);

	Tile* tile = entity.Set<Tile>(Asset, "", isStatic);

	Transform*       transform = entity.Set<Transform>();
	Collider*        collider  = entity.Set<MeshCollider>();
	CollisionObject* object    = isSimulated ? entity.Set<Rigidbody>() : entity.Set<CollisionObject>();

	object->SetCol(collider);
	object->SetTrans(transform);

	entity.Set<Mesh>(tile->m_spriteMesh); // tmep debug

	return entity;
}

IW_PLUGIN_SAND_END
