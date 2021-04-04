#include "Layers/SimpleSandLayer.h"

#include "iw/graphics/Font.h"

size_t __width = 1920;
size_t __height = 1080;

double __cellScale = 2;
size_t __chunkSize = 64 * __cellScale;

iw::Mesh text_stats;
iw::ref<iw::Font> font_stats;

void UpdateStats(int cellUpdateCount, int chunkCount) {
	std::stringstream sb;
	sb.precision(2);
	sb << std::fixed << iw::DeltaTime() * 1000 << "ms\n" << 1 / iw::DeltaTime() << "fps\n" << cellUpdateCount << " cells updated\n" << chunkCount << " chunks";

	font_stats->UpdateMesh(text_stats, sb.str(), .001f, 1);
}

namespace iw {
	SimpleSandLayer::SimpleSandLayer()
		: Layer("Simple Sand")
		, m_world(__chunkSize, __chunkSize, __cellScale) // screen width and height
	{
		__chunkSize /= __cellScale;

		srand(time(nullptr));
	}

	int SimpleSandLayer::Initialize() {
		font_stats = Asset->Load<iw::Font>("fonts/arial.fnt");
		text_stats = font_stats->GenerateMesh("", 1, 1);
		text_stats.SetMaterial(REF<iw::Material>(Asset->Load<iw::Shader>("shaders/font_simple.shader")));
		text_stats.Material()->SetTexture("fontMap", Asset->Load<iw::Texture>("textures/fonts/arial.png"));
		text_stats.Material()->Set("color", iw::Color(1));

		iw::ref<iw::Shader> sandShader = Asset->Load<Shader>("shaders/texture.shader");

		m_sandTexture = REF<iw::Texture>(
			__width  / __cellScale,
			__height / __cellScale,
			iw::TEX_2D,
			iw::RGBA
		);
		m_sandTexture->SetFilter(iw::NEAREST);
		m_sandTexture->CreateColors();
		m_sandTexture->Clear();

		iw::ref<iw::Material> sandScreenMat = REF<iw::Material>(sandShader);
		sandScreenMat->SetTexture("texture", m_sandTexture);

		m_sandMesh = Renderer->ScreenQuad().MakeInstance();
		m_sandMesh.SetMaterial(sandScreenMat);

		// Setup values for cells

		_EMPTY = {
			CellType::EMPTY,
			CellProperties::NONE,
			iw::Color::From255(0, 0, 0, 0)
		};

		_SAND = { 
			CellType::SAND,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE,
			iw::Color::From255(235, 200, 175)
		};

		_WATER = {
			CellType::WATER,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE,
			iw::Color::From255(175, 200, 235)
		};

		_ROCK = {
			CellType::ROCK,
			CellProperties::NONE,
			iw::Color::From255(200, 200, 200)
		};

		Cell::SetDefault(CellType::SAND, _SAND);
		Cell::SetDefault(CellType::WATER, _WATER);
		Cell::SetDefault(CellType::ROCK, _ROCK);

		Tile t = {
			{
				              {2,0},
				       {1,1}, {2,1}, 
				{0,2}, {1,2}, {2,2}, {3,2},
				{0,3}, {1,3}, {2,3}, {3,3},
				{0,4},               {3,4},
				{0,5},               {3,5},
				{0,6},               {3,6},
			},
			200, 200
		};

		m_tiles.push_back(t);


		for (int i = -int(m_world.m_chunkWidth) * 5; i < int(m_world.m_chunkWidth * 6); i++)
			m_world.SetCell(i, m_world.m_chunkHeight * 4, _ROCK);

		for (int i = -100; i < 100; i++)
			m_world.SetCell(m_world.m_chunkHeight * -5 - 50 + i, i*i/200, _ROCK);

		for (int i = -100; i < 100; i++)
			m_world.SetCell(m_world.m_chunkHeight *  5 + 50 + i, i*i/200, _ROCK);

		return 0;
	}

	float timer__asdasd = 0;

	void SimpleSandLayer::PostUpdate() {
		//for (int i = -int(m_world.m_chunkWidth) * 5; i < int(m_world.m_chunkWidth * 6); i++)
		//	if(iw::randf() > .99)
		//		m_world.SetCell(i, m_world.m_chunkHeight*5, _WATER);

		// camera frustrum

		size_t width = m_sandTexture->Width();
		size_t height = m_sandTexture->Height();

		int fx = m_tiles[0].X - width / 2;
		int fy = m_tiles[0].Y - height / 2;
		int fx2 = fx + width;
		int fy2 = fy + height;

		vector2 pos = Mouse::ScreenPos() / m_world.m_scale + iw::vector2(fx, -fy);
		pos.y = height - pos.y;

		if (Mouse::ButtonDown(iw::LMOUSE)) {
			Cell placeMe = _EMPTY;

				 if (Keyboard::KeyDown(iw::S)) placeMe = _SAND;
			else if (Keyboard::KeyDown(iw::W)) placeMe = _WATER;
			else if (Keyboard::KeyDown(iw::R)) placeMe = _ROCK;

			for (int x = pos.x; x < pos.x + 20; x++)
			for (int y = pos.y; y < pos.y + 20; y++) {
				if (!m_world.InBounds(x, y)) continue;
				m_world.SetCell(x, y, placeMe);
			}
		}

		if (Keyboard::KeyDown(iw::LEFT))  m_tiles[0].X -= iw::DeltaTime()*150;
		if (Keyboard::KeyDown(iw::RIGHT)) m_tiles[0].X += iw::DeltaTime()*150;
		if (Keyboard::KeyDown(iw::UP))    m_tiles[0].Y += iw::DeltaTime()*150;
		if (Keyboard::KeyDown(iw::DOWN))  m_tiles[0].Y -= iw::DeltaTime()*150;

		for (Tile& tile : m_tiles) {
			for (auto [x, y] : tile.Positions) {
				x += ceil(tile.X);
				y += ceil(tile.Y);

				if (m_world.InBounds(x, y)) {
					// what happens if the cell is already full?
					m_world.SetCell(x, y, _ROCK);
				}
			}
		}

		// Update cells

		m_world.RemoveEmptyChunks();

		std::mutex mutex;
		std::condition_variable cond;

		auto doForAllChunks = [&](std::function<void(SandChunk*)> func) {
			int chunkCount = m_world.m_chunks.size();
			
			for (SandChunk* chunk : m_world.m_chunks) {
				Task->queue([&, chunk]() {
					func(chunk);

					{ std::unique_lock lock(mutex); chunkCount--; }
					cond.notify_one();
				});
			}

			std::unique_lock lock(mutex);
			cond.wait(lock, [&]() { return chunkCount == 0; });
		};
		
		doForAllChunks([&](SandChunk* chunk) {
			SimpleSandWorker(m_world, chunk).UpdateChunk();
		});

		doForAllChunks([&](SandChunk* chunk) {
			chunk->CommitCells();
		});

		for (SandChunk* chunk : m_world.m_chunks) {
			chunk->UpdateRect();
		}

		// Update the sand texture

		bool _debugShowChunkBounds = Keyboard::KeyDown(iw::C);

		unsigned int* pixels = (unsigned int*)m_sandTexture->Colors();
		m_sandTexture->Clear();

		auto [minCX, minCY] = m_world.GetChunkLocation(fx,  fy);
		auto [maxCX, maxCY] = m_world.GetChunkLocation(fx2, fy2);

		int chunkCount = 0;

		for (int cx = minCX-1; cx <= maxCX; cx++)
		for (int cy = minCY-1; cy <= maxCY; cy++) {
			SandChunk* chunk = m_world.GetChunkDirect({ cx, cy });
			if (!chunk) continue;

			{ std::unique_lock lock(mutex); chunkCount++; }

			Task->queue([&, cx, cy, chunk]() {
				int startY = iw::clamp<int>(fy  - chunk->m_y, 0, chunk->m_height); // Could use diry rect
				int startX = iw::clamp<int>(fx  - chunk->m_x, 0, chunk->m_width);
				int endY   = iw::clamp<int>(fy2 - chunk->m_y, 0, chunk->m_height);
				int endX   = iw::clamp<int>(fx2 - chunk->m_x, 0, chunk->m_width);

				// sand texture
				for (int y = startY; y < endY; y++)
				for (int x = startX; x < endX; x++) {
					int texi = (chunk->m_x + x - fx) + (chunk->m_y + y - fy) * width;

					const Cell& cell = chunk->m_cells[x + y * chunk->m_width];
					if (cell.Type != CellType::EMPTY) {
						pixels[texi] = cell.Color;
					}

					if (_debugShowChunkBounds) {
						if (   (y == chunk->m_minY || y == chunk->m_maxY) && (x >= chunk->m_minX && x <= chunk->m_maxX)
							|| (x == chunk->m_minX || x == chunk->m_maxX) && (y >= chunk->m_minY && y <= chunk->m_maxY))
						{
							pixels[texi] = iw::Color(0, 1, 0);
						}
						
						else if (x % __chunkSize == 0
							 ||  y % __chunkSize == 0)
						{
							pixels[texi] = iw::Color(1, 0, 0);
						}
					}
				}

				{ std::unique_lock lock(mutex); chunkCount--; }
				cond.notify_one();
			});
		}

		// Wait for all chunks to be copied
		{
			std::unique_lock lock(mutex);
			cond.wait(lock, [&]() { return chunkCount == 0; });
		}

		// Mouse cursor

		size_t mouseIndex = size_t(pos.x - fx) + size_t(pos.y - fy) * width;
		if (mouseIndex < width * height) {
			pixels[mouseIndex] = iw::Color(1, 0, 0);
		}
		
		// Remove tile

		for (Tile& tile : m_tiles) {
			for (auto [x, y] : tile.Positions) {
				x += ceil(tile.X);
				y += ceil(tile.Y);

				if (m_world.InBounds(x, y)) {
					// what happens if the cell is no longer part of the tile?
					m_world.SetCell(x, y, _EMPTY);
				}
			}
		}

		//UpdateStats(cellUpdateCount, m_world.m_chunks.size());

		// Draw the sand to the screen

		m_sandTexture->Update(Renderer->Device);

		Renderer->BeginScene(MainScene);
		Renderer->	DrawMesh(iw::Transform(), m_sandMesh);
		//Renderer->	DrawMesh(iw::Transform(iw::vector3(-.95, -.6, 0)), text_stats);
		Renderer->EndScene();


	//	// for vid
	//	{
	//		// Initial
	//		{
	//			for (SandChunk* chunk : m_world.GetAllChunks()) {
	//				SimpleSandWorker(m_world, chunk).UpdateChunk();
	//			}
	//		} // end v2 version

	//		// Has problems, doesnt wait on main thread
	//		{
	//			for (SandChunk* chunk : m_world.GetAllChunks()) {
	//				Task->queue([&, chunk]() {
	//					SimpleSandWorker(m_world, chunk).UpdateChunk();
	//				});
	//			}
	//		} // end v2 version

	//		// Still has problems, chunk boundaries
	//		{
	//			std::mutex mutex;
	//			std::condition_variable cond;
	//			int chunkCount = 0;

	//			for (SandChunk* chunk : m_world.GetAllChunks()) {

	//				{ std::unique_lock lock(mutex); chunkCount++; }

	//				Task->queue([&, chunk]() {
	//					SimpleSandWorker(m_world, chunk).UpdateChunk();

	//					{ std::unique_lock lock(mutex); chunkCount--; }
	//					cond.notify_one();
	//				});
	//			}

	//			// wait for chunks to finish updating
	//			std::unique_lock lock(mutex);
	//			cond.wait(lock, [&]() { return chunkCount == 0; });
	//		} // end v3 version

	//		// Final version
	//		{
	//			std::mutex mutex;
	//			std::condition_variable cond;
	//			int chunkCount = 0;

	//			for (auto& batch : m_world.GetChunkBatches()) {
	//				for (SandChunk* chunk : batch) {

	//					{ std::unique_lock lock(mutex); chunkCount++; }

	//					Task->queue([&, chunk]() {
	//						SimpleSandWorker(m_world, chunk).UpdateChunk();

	//						{ std::unique_lock lock(mutex); chunkCount--; }
	//						cond.notify_one();
	//					});
	//				}

	//				// wait for batch to finish updating
	//				std::unique_lock lock(mutex);
	//				cond.wait(lock, [&]() { return chunkCount == 0; });
	//			}
	//		} // end final version
	//	} // end vid

	}
}

// Simple sand worker

bool SimpleSandWorker::MoveDown(
	size_t x, size_t y,
	const Cell& cell)
{
	for (int i = 1; i > 0; i--) {
		bool down = IsEmpty(x, y - i);
		if (down) {
			MoveCell(x, y, x, y - i);
			return true;
		}
	}

	return false;
}
	
bool SimpleSandWorker::MoveDownSide(
	size_t x, size_t y,
	const Cell& cell)
{
	bool downLeft  = IsEmpty(x - 1, y - 1);
	bool downRight = IsEmpty(x + 1, y - 1);

	ShuffleIfTrue(downLeft, downRight);

	if		(downLeft)	MoveCell(x, y, x - 1, y - 1);
	else if (downRight) MoveCell(x, y, x + 1, y - 1);

	return downLeft || downRight;
}

bool SimpleSandWorker::MoveSide(
	size_t x, size_t y,
	const Cell& cell)
{
	for (int i = 1; i > 0; i--) {
		bool left  = IsEmpty(x - i, y);
		bool right = IsEmpty(x + i, y);

		ShuffleIfTrue(left, right);

			 if (left)  MoveCell(x, y, x - i, y);
		else if (right)	MoveCell(x, y, x + i, y);

		if (left || right) return true;
	}

	return false;
}
