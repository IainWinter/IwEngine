#include "Systems/SandWorldUpdateSystem.h"
#include "plugins/iw/Sand/Workers/SimpleSandWorker.h"

#include "iw/math/iwmath.h"

//void MoveInDirection(
//	int dx, int dy, 
//	int x, int y, 
//	const iw::Cell& cell)
//{
//	// move cell at x,y by dx, dy
//}
//
//void test() {
//	int x; int y; iw::Cell& cell;
//
//	using namespace std::placeholders;
//
//	auto moveUp = std::bind(MoveInDirection,  0,  1, _1, _2, _3);
//	auto moveDn = std::bind(MoveInDirection,  0, -1, _1, _2, _3);
//	auto moveLt = std::bind(MoveInDirection, -1,  1, _1, _2, _3);
//	auto moveRt = std::bind(MoveInDirection,  1,  1, _1, _2, _3);
//
//	moveUp(x, y, cell);
//	moveDn(x, y, cell);
//	moveLt(x, y, cell);
//	moveRt(x, y, cell);
//}

class GameSandWorker : public iw::SimpleSandWorker {
public:
	GameSandWorker(iw::SandWorld& world, iw::SandChunk* chunk) : iw::SimpleSandWorker(world, chunk) {}

	void UpdateCell(int x, int y, iw::Cell& cell) override {

		if (cell.Type == iw::CellType::SMOKE) {
			cell.time += iw::DeltaTime();

			if (cell.time > 10 + iw::randf() * 5) {
				SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
				return;
			}
		}

		if (cell.Props & iw::CellProperties::BURN     && BurnFire(x, y, cell)) {}
		if (cell.Props & iw::CellProperties::CONVEYOR && MoveConveyor(x, y, cell)) {}
		else

		SimpleSandWorker::UpdateCell(x, y, cell);
	}
private:
	bool BurnFire(
		int x, int y,
		iw::Cell& cell)
	{
		cell.time += iw::DeltaTime();

		float r = iw::randf();

		if (cell.time > 5 + r) {
			SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
		}

		//else
		//if (   cell.time > 1
		//	&& IsEmpty(x, y + 1))
		//{
		//	SetCell(x, y + 1, iw::Cell::GetDefault(iw::CellType::FIRE));
		//}

		else {
			for(int xx = -1; xx <= 1; xx++)
			for(int yy = -1; yy <= 1; yy++) {
				float/*&*/ temp = 0;//m_chunk->GetCell<HeatField>(x, y, 2).Tempeture;
				temp = iw::clamp<float>(temp + iw::DeltaTime() * 100, 0, 1000);

				if (xx == 0 && yy == 0) continue; 

				if (   iw::randf() > .99f/*temp > 451*/
					&& InBounds(x + xx, y + yy)
					&& GetCell (x + xx, y + yy).Type == iw::CellType::WOOD)
				{
					SetCell(x + xx, y + yy, iw::Cell::GetDefault(/*iw::randf() > .99f ?*/ iw::CellType::FIRE /*: iw::CellType::SMOKE*/));
					break;
				}
			}
		}

		KeepAlive(x, y);

		return false;
	}

	bool MoveConveyor(
		int x, int y, 
		iw::Cell& cell)
	{
		if (GetCell(x, y + 1).Type == iw::CellType::BELT) return true;

		int i = 1;
		while (GetCell(x, y - i).Type == iw::CellType::BELT) {
			i++;
		}

		for (int j = 1; j <= i; j++) {
			if ( IsEmpty(x,     y + j)) break;
			if (!IsEmpty(x + 1, y + j)) continue;

			iw::Cell& above = GetCell(x, y + j);

			if (above.Props & iw::CellProperties::MOVE_DOWN) {
				/*PushCell*/MoveCell(x, y + j, x + 1, y + j);
			}

			else {
				break;
			}
		}

		return false;
	}
};

float timer__ = 0.0;

void mySandWorldUpdateSystem::Update() {
	// Paste tiles

	//timer__ -= iw::DeltaTime();
	//if (timer__ > 0) {
	//	return;
	//}

	//if (!iw::Keyboard::KeyDown(iw::J)) return;

	//timer__ = .05f;

	// Remove tiles

	Space->Query<myTile>().Each([&](
		auto entity,
		myTile* tile)
	{
		for (auto [x, y] : tile->Positions) {
			x += ceil(tile->X);
			y += ceil(tile->Y);

			if (m_world.InBounds(x, y)) {
				// what happens if the cell is already full?
				m_world.SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
			}
		}		
	});

	Space->Query<myTile>().Each([&](
		auto entity,
		myTile* tile)
	{
		for (auto [x, y] : tile->Positions) {
			x += ceil(tile->X);
			y += ceil(tile->Y);

			if (m_world.InBounds(x, y)) {
				// what happens if the cell is already full?
				m_world.SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
			}
		}		
	});

	// Update cells

	//m_world.RemoveEmptyChunks();

	std::mutex mutex;
	std::condition_variable cond;

	auto doForAllChunks = [&](std::function<void(iw::SandChunk*)> func)
	{		
		for (auto& chunks : m_world.m_batches)
		{
			int chunkCount = chunks.size();
			
			for (iw::SandChunk* chunk : chunks) {
				Task->queue([&, chunk]() {
					func(chunk);

					{ std::unique_lock lock(mutex); chunkCount--; }
					cond.notify_one();
				});
			}

			std::unique_lock lock(mutex);
			cond.wait(lock, [&]() { return chunkCount == 0; });
		}
	};

	//for (int i = 0; i < 4; i++) {
		m_world.m_frameCount += 1;

		doForAllChunks([&](iw::SandChunk* chunk) {
			GameSandWorker(m_world, chunk).UpdateChunk();
		});

		doForAllChunks([&](iw::SandChunk* chunk) {
			chunk->CommitCells();
		});
	//}

	// Update the sand texture

	bool _debugShowChunkBounds = iw::Keyboard::KeyDown(iw::C);

	unsigned int* pixels = (unsigned int*)m_texture->Colors();
	m_texture->Clear();

	auto [minCX, minCY] = m_world.GetChunkLocation(m_fx,  m_fy);
	auto [maxCX, maxCY] = m_world.GetChunkLocation(m_fx2, m_fy2);

	int chunkCount = 0;

	for (int cx = minCX-1; cx <= maxCX; cx++)
	for (int cy = minCY-1; cy <= maxCY; cy++) {
		iw::SandChunk* chunk = m_world.GetChunkDirect({ cx, cy });
		if (!chunk) continue;

		{ std::unique_lock lock(mutex); chunkCount++; }

		Task->queue([&, cx, cy, chunk]() {
			int startY = iw::clamp<int>(m_fy  - chunk->m_y, 0, chunk->m_height); // Could use diry rect
			int startX = iw::clamp<int>(m_fx  - chunk->m_x, 0, chunk->m_width);
			int endY   = iw::clamp<int>(m_fy2 - chunk->m_y, 0, chunk->m_height);
			int endX   = iw::clamp<int>(m_fx2 - chunk->m_x, 0, chunk->m_width);

			iw::Cell* cells = chunk->GetField(iw::SandField::CELL).GetCells<iw::Cell>();
			//HeatField* heat = chunk->GetField().cells;

			// sand texture
			for (int y = startY; y < endY; y++)
			for (int x = startX; x < endX; x++) {
				int texi = (chunk->m_x + x - m_fx) + (chunk->m_y + y - m_fy) * m_texture->Width();

				iw::Cell& cell = cells[x + y * chunk->m_width];
				float/*&*/temp = 0;//heat [x + y * chunk->m_width].Tempeture;

				temp *= (1 -iw::DeltaTime());

				if (_debugShowChunkBounds) {
					if (   (y == chunk->m_minY || y == chunk->m_maxY) && (x >= chunk->m_minX && x <= chunk->m_maxX)
						|| (x == chunk->m_minX || x == chunk->m_maxX) && (y >= chunk->m_minY && y <= chunk->m_maxY))
					{
						pixels[texi] = iw::Color(0, 1, 0).to32();
					}
						
					else 
					if (    x % m_world.m_chunkWidth  == 0
						||  y % m_world.m_chunkHeight == 0)
					{
						int c = m_world.m_batchGridSize;

						int batch = (c + cx % c) % c
								  + (c + cy % c) % c * c;

						iw::Color color;

						switch (batch) {
						case 0: color = iw::Color::From255(255,   0, 255); break;
						case 1: color = iw::Color::From255(255,   0,   0); break;
						case 2: color = iw::Color::From255(255, 128,   0); break;
						case 3: color = iw::Color::From255(255, 255,   0); break;
						case 4: color = iw::Color::From255(128, 255,   0); break;
						case 5: color = iw::Color::From255(0,   255,   0); break;
						case 6: color = iw::Color::From255(0,   255, 128); break;
						case 7: color = iw::Color::From255(0,   128, 255); break;
						case 8: color = iw::Color::From255(0,     0, 255); break;
						}

						pixels[texi] = color.to32();
					}
				}

				if (cell.Type != iw::CellType::EMPTY) {
					glm::vec4 accent = cell.StyleColor.rgba();

					switch (cell.Style) {
						case iw::CellStyle::RANDOM_STATIC: {
							accent *= cell.StyleOffset;
							break;
						}
						case iw::CellStyle::SHIMMER: {
							accent *= sin(cell.StyleOffset + cell.StyleOffset * 5 * iw::TotalTime());
							break;
						}
					}

					accent = iw::lerp(accent, glm::vec4(1), temp / 1000);

					pixels[texi] = iw::Color(cell.Color + accent).to32();
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

	//UpdateStats(cellUpdateCount, m_world.m_chunks.size());

	m_texture->Update(Renderer->Device);
}
